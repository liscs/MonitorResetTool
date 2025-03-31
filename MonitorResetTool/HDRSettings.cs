using Windows.Win32;
using Windows.Win32.Devices.Display;
using Windows.Win32.Foundation;
namespace MonitorResetTool
{
    public unsafe class HDRSettings
    {
        private static void ForEachDisplay(Action<DISPLAYCONFIG_MODE_INFO> func)
        {
            if (PInvoke.GetDisplayConfigBufferSizes(QUERY_DISPLAY_CONFIG_FLAGS.QDC_ONLY_ACTIVE_PATHS, out uint pathCount, out uint modeCount) != WIN32_ERROR.NO_ERROR)
                return;

            DISPLAYCONFIG_PATH_INFO[] pathsArray = new DISPLAYCONFIG_PATH_INFO[(int)pathCount];
            DISPLAYCONFIG_MODE_INFO[] modesArray = new DISPLAYCONFIG_MODE_INFO[(int)modeCount];
            fixed (DISPLAYCONFIG_PATH_INFO* paths = pathsArray)
            fixed (DISPLAYCONFIG_MODE_INFO* modes = modesArray)
            {
                if (PInvoke.QueryDisplayConfig(QUERY_DISPLAY_CONFIG_FLAGS.QDC_ONLY_ACTIVE_PATHS, ref pathCount, paths, ref modeCount, modes, (DISPLAYCONFIG_TOPOLOGY_ID*)0) != WIN32_ERROR.NO_ERROR)
                    return;
            }
            foreach (var path in pathsArray)
            {
                DISPLAYCONFIG_MODE_INFO mode = modesArray[path.targetInfo.Anonymous.modeInfoIdx];
                func(mode);
            }
        }

        // 获取显示器的 HDR 状态
        private static HDRStatus GetDisplayHDRStatus(ref DISPLAYCONFIG_MODE_INFO mode)
        {
            DISPLAYCONFIG_GET_ADVANCED_COLOR_INFO getColorInfo = new();

            getColorInfo.header.type = DISPLAYCONFIG_DEVICE_INFO_TYPE.DISPLAYCONFIG_DEVICE_INFO_GET_ADVANCED_COLOR_INFO;
            getColorInfo.header.size = (uint)sizeof(DISPLAYCONFIG_GET_ADVANCED_COLOR_INFO);
            getColorInfo.header.adapterId.HighPart = mode.adapterId.HighPart;
            getColorInfo.header.adapterId.LowPart = mode.adapterId.LowPart;
            getColorInfo.header.id = mode.id;

            if (PInvoke.DisplayConfigGetDeviceInfo(ref getColorInfo.header) != (int)WIN32_ERROR.NO_ERROR)
                return HDRStatus.Unsupported;

            if (!getColorInfo.Anonymous.Anonymous.advancedColorSupported)
                return HDRStatus.Unsupported;

            return getColorInfo.Anonymous.Anonymous.advancedColorEnabled ? HDRStatus.On : HDRStatus.Off;
        }

        // 获取所有显示器的 HDR 状态
        public static HDRStatus GetWindowsHDRStatus()
        {
            bool anySupported = false;
            bool anyEnabled = false;

            ForEachDisplay(mode =>
                {
                    HDRStatus displayStatus = GetDisplayHDRStatus(ref mode);
                    anySupported |= displayStatus != HDRStatus.Unsupported;
                    anyEnabled |= displayStatus == HDRStatus.On;
                });

            if (anySupported)
                return anyEnabled ? HDRStatus.On : HDRStatus.Off;
            else
                return HDRStatus.Unsupported;
        }

        // 设置单个显示器的 HDR 状态
        private static HDRStatus? SetDisplayHDRStatus(ref DISPLAYCONFIG_MODE_INFO mode, bool enable)
        {
            if (GetDisplayHDRStatus(ref mode) == HDRStatus.Unsupported)
                return null;

            DISPLAYCONFIG_SET_ADVANCED_COLOR_STATE setColorState = new();
            setColorState.header.type = DISPLAYCONFIG_DEVICE_INFO_TYPE.DISPLAYCONFIG_DEVICE_INFO_SET_ADVANCED_COLOR_STATE;
            setColorState.header.size = (uint)sizeof(DISPLAYCONFIG_SET_ADVANCED_COLOR_STATE);
            setColorState.header.adapterId.HighPart = mode.adapterId.HighPart;
            setColorState.header.adapterId.LowPart = mode.adapterId.LowPart;
            setColorState.header.id = mode.id;
            setColorState.Anonymous.Anonymous.enableAdvancedColor = enable;

            if (PInvoke.DisplayConfigSetDeviceInfo(&setColorState.header) != (int)WIN32_ERROR.NO_ERROR)
                return null;

            return GetDisplayHDRStatus(ref mode);
        }

        // 设置所有显示器的 HDR 状态
        public static HDRStatus? SetWindowsHDRStatus(bool enable)
        {
            HDRStatus? status = null;

            ForEachDisplay(mode =>
            {
                var new_status = SetDisplayHDRStatus(ref mode, enable);
                if (new_status == null)
                    return;
                if (status == null)
                    status = new_status;
                else
                    status = (HDRStatus)(Math.Max((int)status, (int)new_status));
            });

            return status;
        }

        // 切换所有显示器的 HDR 状态
        public static HDRStatus? ToggleHDRStatus()
        {
            var status = GetWindowsHDRStatus();
            if (status == HDRStatus.Unsupported)
                return HDRStatus.Unsupported;
            return SetWindowsHDRStatus(status == HDRStatus.Off ? true : false);
        }
    }
}
