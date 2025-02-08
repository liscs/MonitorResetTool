#include "hdr.h"
// ��ȡ������ʾ���� HDR ״̬
extern "C" __declspec(dllexport)hdr::Status GetWindowsHDRStatus() {
	return hdr::GetWindowsHDRStatus();
}

extern "C" __declspec(dllexport) hdr::Status ToggleHDRStatus() {
	auto optionalValue = hdr::ToggleHDRStatus();
	if (optionalValue)
	{
		return optionalValue.value();
	}
	else
	{
		return hdr::Status();
	}
}

extern "C" __declspec(dllexport) hdr::Status SetWindowsHDRStatus(bool enable) {
	auto optionalValue = hdr::SetWindowsHDRStatus(enable);
	if (optionalValue)
	{
		return optionalValue.value();
	}
	else
	{
		return hdr::Status();
	}
}
