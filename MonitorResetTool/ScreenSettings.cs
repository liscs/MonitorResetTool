using System.Runtime.Versioning;
using Windows.Win32;
using Windows.Win32.Graphics.Gdi;
public class ScreenSettings
{
    [SupportedOSPlatform("windows10.0.18362")]
    public static unsafe void SetRefreshRate(uint width, uint height, uint refreshRate)
    {
        DEVMODEW dm = new();
        if (PInvoke.EnumDisplaySettings(null, ENUM_DISPLAY_SETTINGS_MODE.ENUM_CURRENT_SETTINGS, &dm))
        {
            dm.dmDisplayFrequency = refreshRate;
            dm.dmPelsWidth = width;
            dm.dmPelsHeight = height;
            DISP_CHANGE result = PInvoke.ChangeDisplaySettings(dm, 0);
            if (result == 0)
            {
                Console.WriteLine("刷新率设置成功！");
            }
            else
            {
                Console.WriteLine($"刷新率设置失败，错误码：{result}");
            }
        }
        else
        {
            Console.WriteLine("无法获取当前显示设置！");
        }
    }
}

