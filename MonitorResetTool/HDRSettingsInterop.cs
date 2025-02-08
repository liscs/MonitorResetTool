using System.Runtime.InteropServices;

namespace MonitorResetTool;

public partial class HDRSettingsInterop
{

    // 获取所有显示器的 HDR 状态
    [DllImport("hdr.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern Status GetWindowsHDRStatus();

    [DllImport("hdr.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern Status SetWindowsHDRStatus(bool enable);

    [DllImport("hdr.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern Status ToggleHDRStatus();

}