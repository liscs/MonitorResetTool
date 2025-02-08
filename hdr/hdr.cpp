#define NOMINMAX

#include <windows.h>
#include <optional>
#include "hdr.h"
namespace hdr {
	// 获取显示器的 HDR 状态
	Status GetDisplayHDRStatus(const DISPLAYCONFIG_MODE_INFO& mode)
	{
		DISPLAYCONFIG_GET_ADVANCED_COLOR_INFO_2 getColorInfo2 = {};
		getColorInfo2.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_ADVANCED_COLOR_INFO_2;
		getColorInfo2.header.size = sizeof(getColorInfo2);
		getColorInfo2.header.adapterId.HighPart = mode.adapterId.HighPart;
		getColorInfo2.header.adapterId.LowPart = mode.adapterId.LowPart;
		getColorInfo2.header.id = mode.id;

		if (use_win11_24h2_color_functions && DisplayConfigGetDeviceInfo(&getColorInfo2.header) == ERROR_SUCCESS)
		{
			if (!getColorInfo2.highDynamicRangeSupported)
				return Status::Unsupported;

			return getColorInfo2.activeColorMode == DISPLAYCONFIG_ADVANCED_COLOR_MODE_HDR ? Status::On : Status::Off;
		}

		DISPLAYCONFIG_GET_ADVANCED_COLOR_INFO getColorInfo = {};
		getColorInfo.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_ADVANCED_COLOR_INFO;
		getColorInfo.header.size = sizeof(getColorInfo);
		getColorInfo.header.adapterId.HighPart = mode.adapterId.HighPart;
		getColorInfo.header.adapterId.LowPart = mode.adapterId.LowPart;
		getColorInfo.header.id = mode.id;

		if (DisplayConfigGetDeviceInfo(&getColorInfo.header) != ERROR_SUCCESS)
			return Status::Unsupported;

		if (!getColorInfo.advancedColorSupported)
			return Status::Unsupported;

		return getColorInfo.advancedColorEnabled ? Status::On : Status::Off;
	}

	// 获取所有显示器的 HDR 状态
	Status GetWindowsHDRStatus()
	{
		bool anySupported = false;
		bool anyEnabled = false;

		ForEachDisplay([&](const DISPLAYCONFIG_MODE_INFO& mode) {
			Status displayStatus = GetDisplayHDRStatus(mode);
			anySupported |= displayStatus != Status::Unsupported;
			anyEnabled |= displayStatus == Status::On;
			});

		if (anySupported)
			return anyEnabled ? Status::On : Status::Off;
		else
			return Status::Unsupported;
	}

	// 设置单个显示器的 HDR 状态
	std::optional<Status> SetDisplayHDRStatus(const DISPLAYCONFIG_MODE_INFO& mode, bool enable)
	{
		if (GetDisplayHDRStatus(mode) == Status::Unsupported)
			return std::nullopt;

		DISPLAYCONFIG_SET_HDR_STATE setHdrState = {};
		setHdrState.header.type = DISPLAYCONFIG_DEVICE_INFO_SET_HDR_STATE;
		setHdrState.header.size = sizeof(setHdrState);
		setHdrState.header.adapterId.HighPart = mode.adapterId.HighPart;
		setHdrState.header.adapterId.LowPart = mode.adapterId.LowPart;
		setHdrState.header.id = mode.id;
		setHdrState.enableHdr = enable;

		if (use_win11_24h2_color_functions && DisplayConfigSetDeviceInfo(&setHdrState.header) == ERROR_SUCCESS)
			return GetDisplayHDRStatus(mode);

		DISPLAYCONFIG_SET_ADVANCED_COLOR_STATE setColorState = {};
		setColorState.header.type = DISPLAYCONFIG_DEVICE_INFO_SET_ADVANCED_COLOR_STATE;
		setColorState.header.size = sizeof(setColorState);
		setColorState.header.adapterId.HighPart = mode.adapterId.HighPart;
		setColorState.header.adapterId.LowPart = mode.adapterId.LowPart;
		setColorState.header.id = mode.id;
		setColorState.enableAdvancedColor = enable;

		if (DisplayConfigSetDeviceInfo(&setColorState.header) != ERROR_SUCCESS)
			return std::nullopt;

		return GetDisplayHDRStatus(mode);
	}

	// 设置所有显示器的 HDR 状态
	std::optional<Status> SetWindowsHDRStatus(bool enable)
	{
		std::optional<Status> status;

		ForEachDisplay([&](const DISPLAYCONFIG_MODE_INFO& mode) {
			auto new_status = SetDisplayHDRStatus(mode, enable);
			if (!new_status)
				return;

			if (!status)
				status = *new_status;
			else
				status = static_cast<Status>(std::max(static_cast<int>(*status), static_cast<int>(*new_status)));
			});

		return status;
	}

	// 切换所有显示器的 HDR 状态
	std::optional<Status> ToggleHDRStatus()
	{
		auto status = GetWindowsHDRStatus();
		if (status == Status::Unsupported)
			return Status::Unsupported;
		return SetWindowsHDRStatus(status == Status::Off ? true : false);
	}

} // namespace hdr