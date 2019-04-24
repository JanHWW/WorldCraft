#pragma once
constexpr int TexturePixelSize = 0x20;
constexpr int DataSize = 0x200;
constexpr float TextSize = (float)TexturePixelSize / DataSize;
constexpr float LastRowY = 1 - TextSize;