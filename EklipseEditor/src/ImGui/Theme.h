#pragma once

namespace Eklipse
{
    enum class Theme
    {
        Light,
        Dark,
        DarkGrey,
        DarkBlue,
    };

    String ThemeToString(Theme theme);
    Theme StringToTheme(const String& theme);

    void SetTheme(Theme theme);
}