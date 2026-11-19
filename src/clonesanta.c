#include "clonesanta.h"

// Helper function to draw a layered pine tree with snow caps
void CLONESANTA_DRAWPINETREE(float x, float y, float scale) 
{
    Color trunkBrown = { 92, 58, 33, 255 };
    Color darkPine   = { 18, 68, 42, 255 };
    Color midPine    = { 26, 92, 56, 255 };
    Color snowCap    = { 240, 248, 255, 255 };

    // Trunk
    DrawRectangle(x - 10 * scale, y - 30 * scale, 20 * scale, 35 * scale, trunkBrown);

    // 3 Overlapping Foliage Tiers
    for (int i = 0; i < 3; i++) {
        float tierY = y - (25 + i * 32) * scale;
        float width = (55 - i * 10) * scale;
        float height = 50 * scale;

        // Main green triangle
        DrawTriangle(
            (Vector2){ x - width, tierY },
            (Vector2){ x + width, tierY },
            (Vector2){ x, tierY - height },
            (i % 2 == 0) ? darkPine : midPine
        );

        // Snow cap on the tip of each tier
        DrawTriangle(
            (Vector2){ x - width * 0.35f, tierY - height * 0.65f },
            (Vector2){ x + width * 0.35f, tierY - height * 0.65f },
            (Vector2){ x, tierY - height },
            snowCap
        );
    }
}

// Animated Christmas Background Function
void CLONESANTA_DRAWBACKGROUND(int screenWidth, int screenHeight, float time) 
{
    // 1. Night Sky Gradient
    Color skyTop    = { 12, 22, 44, 255 };
    Color skyBottom = { 34, 58, 94, 255 };
    DrawRectangleGradientV(0, 0, screenWidth, screenHeight, skyTop, skyBottom);

    // 2. Twinkling Stars
    for (int i = 0; i < 45; i++) {
        float sx = (float)((i * 137) % screenWidth);
        float sy = (float)((i * 89) % (int)(screenHeight * 0.55f));
        float twinkle = 0.4f + 0.6f * sinf(time * 2.5f + i * 1.7f);
        float radius = (i % 3 == 0) ? 2.2f : 1.3f;
        DrawCircleV((Vector2){ sx, sy }, radius, Fade(RAYWHITE, twinkle));
    }

    // 3. Full Glowing Moon
    Vector2 moonPos = { screenWidth - 130.0f, 100.0f };
    DrawCircleV(moonPos, 52, Fade((Color){ 255, 248, 220, 255 }, 0.12f)); // Outer glow
    DrawCircleV(moonPos, 40, Fade((Color){ 255, 248, 220, 255 }, 0.25f)); // Inner glow
    DrawCircleV(moonPos, 32, (Color){ 255, 250, 230, 255 });              // Moon body

    // 4. Distant Rolling Snowy Hills
    Color hillBack  = { 195, 215, 235, 255 };
    Color hillFront = { 225, 238, 250, 255 };
    DrawCircle(180, screenHeight + 120, 340, hillBack);
    DrawCircle(650, screenHeight + 140, 380, hillBack);
    DrawCircle(420, screenHeight + 190, 420, hillFront);

    // 5. Pine Trees Scattered Along the Hills
    CLONESANTA_DRAWPINETREE(90,  screenHeight - 95, 0.9f);
    CLONESANTA_DRAWPINETREE(165, screenHeight - 80, 1.2f);
    CLONESANTA_DRAWPINETREE(690, screenHeight - 85, 1.15f);
    CLONESANTA_DRAWPINETREE(755, screenHeight - 105, 0.8f);

    // 6. Cozy Cabin in the Background (Left side)
    float cabX = 235.0f;
    float cabY = screenHeight - 125.0f;
    // Animated chimney smoke puffs
    for(int s = 0; s < 4; s++)
    {
        float cycle = fmodf(time * 0.4f + s * 0.25f, 1.0f);
        float smokeX = cabX + 48 + sinf(time * 2.0f + s) * 8.0f + cycle * 15.0f;
        float smokeY = (cabY - 35) - cycle * 55.0f;
        DrawCircleV((Vector2){ smokeX, smokeY }, 5.0f + cycle * 8.0f, Fade(LIGHTGRAY, (1.0f - cycle) * 0.5f));
    }
    // Chimney
    DrawRectangle(cabX + 42, cabY - 32, 12, 24, (Color){ 130, 50, 45, 255 });
    // Cabin walls
    DrawRectangle(cabX, cabY, 65, 42, (Color){ 115, 68, 40, 255 });
    // Snow-covered roof
    DrawTriangle(
        (Vector2){ cabX - 8, cabY },
        (Vector2){ cabX + 73, cabY },
        (Vector2){ cabX + 32.5f, cabY - 28 },
        (Color){ 245, 250, 255, 255 }
    );
    // Warm glowing window + window crossbars
    float windowGlow = 0.85f + 0.15f * sinf(time * 6.0f); // Subtle fireplace flicker
    DrawRectangle(cabX + 14, cabY + 12, 16, 16, Fade((Color){ 255, 200, 70, 255 }, windowGlow));
    DrawLine(cabX + 22, cabY + 12, cabX + 22, cabY + 28, (Color){ 90, 50, 30, 255 });
    DrawLine(cabX + 14, cabY + 20, cabX + 30, cabY + 20, (Color){ 90, 50, 30, 255 });
    // Cabin door
    DrawRectangle(cabX + 40, cabY + 16, 14, 26, (Color){ 80, 45, 25, 255 });

    // 7. Foreground Snow Ground
    DrawRectangle(0, screenHeight - 90, screenWidth, 90, (Color){ 242, 247, 255, 255 });

    // 8. Festive String Lights Draped Across the Top
    int bulbCount = 18;
    float spacing = (float)screenWidth / (bulbCount - 1);
    Color bulbColors[] = { RED, GOLD, GREEN, SKYBLUE };

    for (int i = 0; i < bulbCount; i++) {
        float bx = i * spacing;
        // Parabolic sag between anchors
        float sag = sinf((float)i / (bulbCount - 1) * PI) * 28.0f;
        float by = 12.0f + sag;

        if (i < bulbCount - 1) {
            float nextBx = (i + 1) * spacing;
            float nextSag = sinf((float)(i + 1) / (bulbCount - 1) * PI) * 28.0f;
            DrawLineEx((Vector2){ bx, by }, (Vector2){ nextBx, 12.0f + nextSag }, 2.0f, (Color){ 30, 35, 45, 255 });
        }

        // Alternating twinkle glow
        Color c = bulbColors[i % 4];
        bool isLit = ((int)(time * 2.5f) + i) % 2 == 0;
        if (isLit) {
            DrawCircleV((Vector2){ bx, by + 6 }, 11.0f, Fade(c, 0.3f));
        }
        DrawCircleV((Vector2){ bx, by + 6 }, 5.0f, isLit ? c : Fade(c, 0.45f));
    }

    // 9. Multi-Layered Swaying Snowfall
    for (int i = 0; i < 90; i++) {
        // Layer 0: far/slow/small, Layer 1: mid, Layer 2: close/fast/large
        int layer = i % 3;
        float speed = 35.0f + layer * 28.0f;
        float radius = 1.5f + layer * 1.1f;
        float alpha = 0.45f + layer * 0.22f;

        float baseX = (float)((i * 97) % screenWidth);
        float sway = sinf(time * (1.2f + layer * 0.4f) + i) * (12.0f + layer * 6.0f);
        float snowX = fmodf(baseX + sway + screenWidth, (float)screenWidth);
        float snowY = fmodf((float)(i * 67) + time * speed, (float)screenHeight);

        DrawCircleV((Vector2){ snowX, snowY }, radius, Fade(WHITE, alpha));
    }
}

// Helper function to draw a layered pine tree with snow caps & warm lights
void CLONESANTA_DRAWPINETREE2(float x, float y, float scale)
{
    Color trunkBrown = { 82, 48, 28, 255 };
    Color darkPine   = { 14, 58, 48, 255 };
    Color midPine    = { 24, 84, 66, 255 };
    Color snowCap    = { 235, 246, 255, 255 };
    Color ornament   = { 255, 195, 75, 255 };

    // Trunk
    DrawRectangle(x - 10 * scale, y - 30 * scale, 20 * scale, 35 * scale, trunkBrown);

    // 3 Overlapping Foliage Tiers
    for (int i = 0; i < 3; i++) {
        float tierY  = y - (25 + i * 32) * scale;
        float width  = (55 - i * 10) * scale;
        float height = 50 * scale;

        DrawTriangle(
            (Vector2){ x - width, tierY },
            (Vector2){ x + width, tierY },
            (Vector2){ x, tierY - height },
            (i % 2 == 0) ? darkPine : midPine
        );

        DrawTriangle(
            (Vector2){ x - width * 0.35f, tierY - height * 0.65f },
            (Vector2){ x + width * 0.35f, tierY - height * 0.65f },
            (Vector2){ x, tierY - height },
            snowCap
        );

        DrawCircleV((Vector2){ x - width * 0.45f, tierY - height * 0.25f }, 2.4f * scale, ornament);
        DrawCircleV((Vector2){ x + width * 0.45f, tierY - height * 0.25f }, 2.4f * scale, ornament);
    }

    DrawCircleV((Vector2){ x, y - (25 + 2 * 32 + 52) * scale }, 3.2f * scale, Fade(ornament, 0.85f));
}

// Helper to draw a snow-roofed holiday house with a glowing window, door, and optional chimney smoke
void CLONESANTA_DRAWHOLESOMEHOUSE2(
    float x,
    float y,
    float w,
    float h,
    float roofH,
    Color wallColor,
    bool hasChimney,
    float time,
    float phase
)
{
    if (hasChimney) {
        float chimX = x + w * 0.72f;
        float chimY = y - roofH * 0.65f;
        for (int s = 0; s < 4; s++) {
            float cycle  = fmodf(time * 0.35f + s * 0.25f + phase, 1.0f);
            float smokeX = chimX + 5.0f + sinf(time * 1.8f + s + phase * 3.0f) * 7.0f + cycle * 14.0f;
            float smokeY = (chimY - 16.0f) - cycle * 45.0f;
            DrawCircleV((Vector2){ smokeX, smokeY }, 3.8f + cycle * 6.5f, Fade((Color){ 235, 245, 255, 255 }, (1.0f - cycle) * 0.40f));
        }
        DrawRectangle(chimX, chimY - 14.0f, 9.0f, 18.0f, (Color){ 135, 52, 46, 255 });
        DrawRectangle(chimX - 1.5f, chimY - 16.0f, 12.0f, 4.0f, (Color){ 244, 250, 255, 255 });
    }

    // Main walls
    DrawRectangle(x, y, w, h, wallColor);

    // Pitched roof + snow layer
    DrawTriangle(
        (Vector2){ x - w * 0.12f, y },
        (Vector2){ x + w * 1.12f, y },
        (Vector2){ x + w * 0.5f, y - roofH },
        (Color){ 72, 44, 34, 255 }
    );
    DrawTriangle(
        (Vector2){ x - w * 0.10f, y - roofH * 0.12f },
        (Vector2){ x + w * 1.10f, y - roofH * 0.12f },
        (Vector2){ x + w * 0.5f, y - roofH },
        (Color){ 242, 249, 255, 255 }
    );

    // Warm flickering window
    float glow = 0.82f + 0.18f * sinf(time * 5.0f + phase * 4.0f);
    float winX = x + w * 0.18f;
    float winY = y + h * 0.30f;
    float winW = w * 0.28f;
    float winH = h * 0.36f;
    DrawRectangle(winX, winY, winW, winH, Fade((Color){ 255, 196, 68, 255 }, glow));
    DrawLine(winX + winW * 0.5f, winY, winX + winW * 0.5f, winY + winH, (Color){ 75, 42, 24, 255 });
    DrawLine(winX, winY + winH * 0.5f, winX + winW, winY + winH * 0.5f, (Color){ 75, 42, 24, 255 });

    // Door
    DrawRectangle(x + w * 0.60f, y + h * 0.36f, w * 0.22f, h * 0.64f, (Color){ 72, 38, 22, 255 });
}

// Alternative Animated Christmas Background: Snowy Village Valley with Aurora
void CLONESANTA_DRAWBACKGROUND2(int screenWidth, int screenHeight, float time)
{
    // 1. Deep Arctic Twilight Gradient
    Color skyTop    = { 7, 14, 30, 255 };
    Color skyBottom = { 22, 46, 74, 255 };
    DrawRectangleGradientV(0, 0, screenWidth, screenHeight, skyTop, skyBottom);

    // 2. Shimmering Aurora Ribbons Across the Upper Sky
    for (int band = 0; band < 3; band++) {
        float baseY = screenHeight * (0.12f + band * 0.055f);
        Color auroraCol = (band == 0)
            ? (Color){ 65, 245, 175, 255 }
            : (band == 1)
                ? (Color){ 70, 190, 255, 255 }
                : (Color){ 175, 110, 255, 255 };

        for (int x = 0; x < screenWidth; x += 12) {
            float wave1 = sinf(x * 0.012f + time * (0.9f + band * 0.25f) + band * 2.0f) * 16.0f;
            float wave2 = cosf(x * 0.025f - time * 0.6f) * 7.0f;
            float ay = baseY + wave1 + wave2;
            float pulse = 0.08f + 0.07f * sinf(time * 1.6f + x * 0.02f + band);

            DrawCircleV((Vector2){ (float)x, ay }, 25.0f, Fade(auroraCol, pulse));
            DrawCircleV((Vector2){ (float)x, ay + 13.0f }, 17.0f, Fade(auroraCol, pulse * 0.6f));
        }
    }

    // 3. Crisp Twinkling Stars
    for (int i = 0; i < 50; i++) {
        float sx = (float)((i * 151) % screenWidth);
        float sy = (float)((i * 83) % (int)(screenHeight * 0.48f));
        float twinkle = 0.35f + 0.65f * sinf(time * 3.0f + i * 1.9f);
        float radius = (i % 4 == 0) ? 2.1f : 1.2f;
        DrawCircleV((Vector2){ sx, sy }, radius, Fade(RAYWHITE, twinkle));
    }

    // 4. Crescent Winter Moon
    /*Vector2 moonPos = { screenWidth * 0.50f, 82.0f };
    DrawCircleV(moonPos, 42, Fade((Color){ 210, 245, 255, 255 }, 0.10f));
    DrawCircleV(moonPos, 31, Fade((Color){ 210, 245, 255, 255 }, 0.22f));
    DrawCircleV(moonPos, 23, (Color){ 238, 250, 255, 255 });
    DrawCircleV((Vector2){ moonPos.x + 9.0f, moonPos.y - 5.0f }, 20, skyTop);*/

    // 5. Mountain Ridge & Twin Valley Hills (forming a central valley dip)
    Color peakFar   = { 36, 58, 88, 255 };
    Color hillLeft  = { 176, 204, 230, 255 };
    Color hillRight = { 190, 216, 238, 255 };
    Color hillFront = { 216, 236, 252, 255 };

    DrawTriangle((Vector2){ 10,  screenHeight - 85 }, (Vector2){ 290, screenHeight - 85 }, (Vector2){ 150, screenHeight - 250 }, peakFar);
    DrawTriangle((Vector2){ 230, screenHeight - 85 }, (Vector2){ 540, screenHeight - 85 }, (Vector2){ 385, screenHeight - 275 }, peakFar);
    DrawTriangle((Vector2){ 490, screenHeight - 85 }, (Vector2){ 790, screenHeight - 85 }, (Vector2){ 640, screenHeight - 245 }, peakFar);

    // Left and right rolling hills with a lower valley saddle in the middle
    DrawCircle(135, screenHeight + 135, 340, hillLeft);
    DrawCircle(665, screenHeight + 135, 340, hillRight);
    DrawCircle(400, screenHeight + 235, 420, hillFront);

    // Subtle frozen valley stream ribbon curving down the center
    for (int step = 0; step < 10; step++) {
        float t = step / 9.0f;
        float streamX = screenWidth * 0.5f + sinf(t * 3.5f) * 28.0f;
        float streamY = (screenHeight - 135.0f) + t * 50.0f;
        float streamW = 14.0f + t * 24.0f;
        DrawEllipse(streamX, streamY, streamW, 5.5f, Fade((Color){ 135, 195, 235, 255 }, 0.55f));
    }

    // 6. Back Ridge Pine Trees (smaller, further up the slopes)
    CLONESANTA_DRAWPINETREE(42,  screenHeight - 132, 0.65f);
    CLONESANTA_DRAWPINETREE(188, screenHeight - 138, 0.70f);
    CLONESANTA_DRAWPINETREE(332, screenHeight - 122, 0.58f);
    CLONESANTA_DRAWPINETREE(472, screenHeight - 124, 0.60f);
    CLONESANTA_DRAWPINETREE(615, screenHeight - 136, 0.68f);
    CLONESANTA_DRAWPINETREE(758, screenHeight - 130, 0.65f);

    // 7. Three Village Houses Across the Valley & Slopes
    // Left chalet
    CLONESANTA_DRAWHOLESOMEHOUSE2(
        78.0f, screenHeight - 124.0f, 56.0f, 34.0f, 26.0f,
        (Color){ 118, 68, 44, 255 }, true, time, 0.1f
    );

    // Center-right bakery with striped awning
    float bakX = 485.0f;
    float bakY = screenHeight - 128.0f;
    CLONESANTA_DRAWHOLESOMEHOUSE2(
        bakX, bakY, 72.0f, 40.0f, 28.0f,
        (Color){ 136, 76, 44, 255 }, true, time, 0.55f
    );
    int stripes = 6;
    float awningW = 34.0f / stripes;
    for (int st = 0; st < stripes; st++) {
        Color stripeCol = (st % 2 == 0) ? (Color){ 195, 42, 48, 255 } : (Color){ 248, 248, 250, 255 };
        DrawRectangle(bakX + 10.0f + st * awningW, bakY + 8.0f, awningW + 0.5f, 7.0f, stripeCol);
    }

    // Right hillside cottage
    CLONESANTA_DRAWHOLESOMEHOUSE2(
        635.0f, screenHeight - 122.0f, 54.0f, 34.0f, 24.0f,
        (Color){ 106, 64, 52, 255 }, false, time, 0.8f
    );

    // 8. Foreground Pine Trees Framing Both Sides
    CLONESANTA_DRAWPINETREE2(45,  screenHeight - 88, 1.05f);
    CLONESANTA_DRAWPINETREE2(156, screenHeight - 78, 0.82f);
    CLONESANTA_DRAWPINETREE2(248, screenHeight - 82, 0.72f);
    CLONESANTA_DRAWPINETREE2(585, screenHeight - 82, 0.76f);
    CLONESANTA_DRAWPINETREE2(705, screenHeight - 86, 0.95f);
    CLONESANTA_DRAWPINETREE2(768, screenHeight - 94, 1.15f);

    // 9. Foreground Snow Ground
    DrawRectangle(0, screenHeight - 90, screenWidth, 90, (Color){ 238, 246, 255, 255 });

    // 10. Festive Lantern & Icicle Garland Along Top Edge
    int nodeCount = 16;
    float spacing = (float)screenWidth / (nodeCount - 1);
    Color lanternColors[] = {
        (Color){ 255, 90, 95, 255 },
        (Color){ 255, 210, 80, 255 },
        (Color){ 95, 225, 175, 255 },
        (Color){ 130, 190, 255, 255 }
    };

    for (int i = 0; i < nodeCount; i++) {
        float lx  = i * spacing;
        float sag = sinf((float)i / (nodeCount - 1) * PI) * 24.0f;
        float ly  = 10.0f + sag;

        if (i < nodeCount - 1) {
            float nextLx  = (i + 1) * spacing;
            float nextSag = sinf((float)(i + 1) / (nodeCount - 1) * PI) * 24.0f;
            DrawLineEx((Vector2){ lx, ly }, (Vector2){ nextLx, 10.0f + nextSag }, 2.0f, (Color){ 45, 58, 78, 255 });
        }

        if (i % 2 == 1) {
            DrawTriangle(
                (Vector2){ lx - 3.0f, ly },
                (Vector2){ lx + 3.0f, ly },
                (Vector2){ lx, ly + 12.0f },
                Fade((Color){ 185, 230, 255, 255 }, 0.75f)
            );
        }

        Color col = lanternColors[i % 4];
        bool lit = ((int)(time * 2.0f) + i) % 2 == 0;
        if (lit) {
            DrawCircleV((Vector2){ lx, ly + 5.0f }, 10.0f, Fade(col, 0.28f));
        }
        DrawCircleV((Vector2){ lx, ly + 5.0f }, 4.5f, lit ? col : Fade(col, 0.40f));
    }

    // 11. Multi-Layered Swaying Snowfall
    for (int i = 0; i < 105; i++) {
        int layer    = i % 3;
        float speed  = 30.0f + layer * 26.0f;
        float radius = 1.4f + layer * 1.15f;
        float alpha  = 0.40f + layer * 0.24f;

        float baseX = (float)((i * 109) % screenWidth);
        float sway  = sinf(time * (1.1f + layer * 0.45f) + i * 0.8f) * (14.0f + layer * 6.0f);
        float snowX = fmodf(baseX + sway + screenWidth, (float)screenWidth);
        float snowY = fmodf((float)(i * 73) + time * speed, (float)screenHeight);

        DrawCircleV((Vector2){ snowX, snowY }, radius, Fade(WHITE, alpha));
    }
}



// Helper function to draw a layered pine tree with snow caps & warm lights
void CLONESANTA_DRAWPINETREE3(float x, float y, float scale)
{
    Color trunkBrown = { 82, 48, 28, 255 };
    Color darkPine   = { 14, 58, 48, 255 };
    Color midPine    = { 24, 84, 66, 255 };
    Color snowCap    = { 235, 246, 255, 255 };
    Color ornament   = { 255, 195, 75, 255 };

    // Trunk
    DrawRectangle(x - 10 * scale, y - 30 * scale, 20 * scale, 35 * scale, trunkBrown);

    // 3 Overlapping Foliage Tiers
    for (int i = 0; i < 3; i++) {
        float tierY  = y - (25 + i * 32) * scale;
        float width  = (55 - i * 10) * scale;
        float height = 50 * scale;

        DrawTriangle(
            (Vector2){ x - width, tierY },
            (Vector2){ x + width, tierY },
            (Vector2){ x, tierY - height },
            (i % 2 == 0) ? darkPine : midPine
        );

        DrawTriangle(
            (Vector2){ x - width * 0.35f, tierY - height * 0.65f },
            (Vector2){ x + width * 0.35f, tierY - height * 0.65f },
            (Vector2){ x, tierY - height },
            snowCap
        );

        DrawCircleV((Vector2){ x - width * 0.45f, tierY - height * 0.25f }, 2.4f * scale, ornament);
        DrawCircleV((Vector2){ x + width * 0.45f, tierY - height * 0.25f }, 2.4f * scale, ornament);
    }

    DrawCircleV((Vector2){ x, y - (25 + 2 * 32 + 52) * scale }, 3.2f * scale, Fade(ornament, 0.85f));
}

// Helper to draw a snow-roofed holiday house perched along the upper ridge
void CLONESANTA_DRAWHOLESOMEHOUSE3(
    float x,
    float y,
    float w,
    float h,
    float roofH,
    Color wallColor,
    bool hasChimney,
    float time,
    float phase
)
{
    if (hasChimney) {
        float chimX = x + w * 0.72f;
        float chimY = y - roofH * 0.65f;
        for (int s = 0; s < 4; s++) {
            float cycle  = fmodf(time * 0.35f + s * 0.25f + phase, 1.0f);
            float smokeX = chimX + 5.0f + sinf(time * 1.8f + s + phase * 3.0f) * 7.0f + cycle * 14.0f;
            float smokeY = (chimY - 16.0f) - cycle * 45.0f;
            DrawCircleV((Vector2){ smokeX, smokeY }, 3.8f + cycle * 6.5f, Fade((Color){ 235, 245, 255, 255 }, (1.0f - cycle) * 0.40f));
        }
        DrawRectangle(chimX, chimY - 14.0f, 9.0f, 18.0f, (Color){ 135, 52, 46, 255 });
        DrawRectangle(chimX - 1.5f, chimY - 16.0f, 12.0f, 4.0f, (Color){ 244, 250, 255, 255 });
    }

    // Main walls
    DrawRectangle(x, y, w, h, wallColor);

    // Pitched roof + snow layer
    DrawTriangle(
        (Vector2){ x - w * 0.12f, y },
        (Vector2){ x + w * 1.12f, y },
        (Vector2){ x + w * 0.5f, y - roofH },
        (Color){ 72, 44, 34, 255 }
    );
    DrawTriangle(
        (Vector2){ x - w * 0.10f, y - roofH * 0.12f },
        (Vector2){ x + w * 1.10f, y - roofH * 0.12f },
        (Vector2){ x + w * 0.5f, y - roofH },
        (Color){ 242, 249, 255, 255 }
    );

    // Warm flickering window
    float glow = 0.82f + 0.18f * sinf(time * 5.0f + phase * 4.0f);
    float winX = x + w * 0.18f;
    float winY = y + h * 0.30f;
    float winW = w * 0.28f;
    float winH = h * 0.36f;
    DrawRectangle(winX, winY, winW, winH, Fade((Color){ 255, 196, 68, 255 }, glow));
    DrawLine(winX + winW * 0.5f, winY, winX + winW * 0.5f, winY + winH, (Color){ 75, 42, 24, 255 });
    DrawLine(winX, winY + winH * 0.5f, winX + winW, winY + winH * 0.5f, (Color){ 75, 42, 24, 255 });

    // Door
    DrawRectangle(x + w * 0.60f, y + h * 0.36f, w * 0.22f, h * 0.64f, (Color){ 72, 38, 22, 255 });
}

// Low-Angle Perspective: Looking Up the Snowy Slope Toward Ridge Houses, Tall Pines & Aurora
void CLONESANTA_DRAWBACKGROUND3(int screenWidth, int screenHeight, float time)
{
    // 1. Deep Arctic Twilight Gradient (more sky visible from low angle)
    Color skyTop    = { 6, 12, 26, 255 };
    Color skyBottom = { 24, 50, 80, 255 };
    DrawRectangleGradientV(0, 0, screenWidth, screenHeight, skyTop, skyBottom);

    // 2. High-Arching Aurora Ribbons Across the Upper Sky
    for (int band = 0; band < 3; band++) {
        float baseY = screenHeight * (0.10f + band * 0.05f);
        Color auroraCol = (band == 0)
            ? (Color){ 65, 245, 175, 255 }
            : (band == 1)
                ? (Color){ 70, 190, 255, 255 }
                : (Color){ 175, 110, 255, 255 };

        for (int x = 0; x < screenWidth; x += 12) {
            float arch  = -sinf(((float)x / screenWidth) * PI) * 22.0f; // Arches upward toward center
            float wave1 = sinf(x * 0.012f + time * (0.9f + band * 0.25f) + band * 2.0f) * 15.0f;
            float wave2 = cosf(x * 0.025f - time * 0.6f) * 6.0f;
            float ay    = baseY + arch + wave1 + wave2;
            float pulse = 0.08f + 0.07f * sinf(time * 1.6f + x * 0.02f + band);

            DrawCircleV((Vector2){ (float)x, ay }, 26.0f, Fade(auroraCol, pulse));
            DrawCircleV((Vector2){ (float)x, ay + 14.0f }, 18.0f, Fade(auroraCol, pulse * 0.6f));
        }
    }

    // 3. Crisp Twinkling Stars
    for (int i = 0; i < 55; i++) {
        float sx = (float)((i * 151) % screenWidth);
        float sy = (float)((i * 83) % (int)(screenHeight * 0.44f));
        float twinkle = 0.35f + 0.65f * sinf(time * 3.0f + i * 1.9f);
        float radius = (i % 4 == 0) ? 2.1f : 1.2f;
        DrawCircleV((Vector2){ sx, sy }, radius, Fade(RAYWHITE, twinkle));
    }

    // 4. Crescent Winter Moon High in the Sky
    Vector2 moonPos = { screenWidth * 0.52f, 70.0f };
    DrawCircleV(moonPos, 42, Fade((Color){ 210, 245, 255, 255 }, 0.10f));
    DrawCircleV(moonPos, 31, Fade((Color){ 210, 245, 255, 255 }, 0.22f));
    DrawCircleV(moonPos, 23, (Color){ 238, 250, 255, 255 });
    DrawCircleV((Vector2){ moonPos.x + 9.0f, moonPos.y - 5.0f }, 20, skyTop);

    // 5. Distant Mountain Ridge Lower in Frame (camera tilted upward)
    Color peakFar   = { 34, 56, 86, 255 };
    Color hillUpper = { 166, 196, 224, 255 };
    Color hillLower = { 216, 236, 252, 255 };

    DrawTriangle((Vector2){ 15,  screenHeight - 135 }, (Vector2){ 285, screenHeight - 135 }, (Vector2){ 150, screenHeight - 235 }, peakFar);
    DrawTriangle((Vector2){ 240, screenHeight - 135 }, (Vector2){ 550, screenHeight - 135 }, (Vector2){ 395, screenHeight - 255 }, peakFar);
    DrawTriangle((Vector2){ 500, screenHeight - 135 }, (Vector2){ 785, screenHeight - 135 }, (Vector2){ 642, screenHeight - 230 }, peakFar);

    // Rising hillside sweeping up from lower ground to upper ridge
    DrawCircle(180, screenHeight + 90, 360, hillUpper);
    DrawCircle(620, screenHeight + 95, 360, hillUpper);
    DrawCircle(400, screenHeight + 175, 430, hillLower);

    // 6. Upper Ridge Pine Trees (smaller, sitting along the crest)
    CLONESANTA_DRAWPINETREE3(36,  screenHeight - 168, 0.55f);
    CLONESANTA_DRAWPINETREE3(225, screenHeight - 172, 0.58f);
    CLONESANTA_DRAWPINETREE3(345, screenHeight - 162, 0.52f);
    CLONESANTA_DRAWPINETREE3(465, screenHeight - 164, 0.54f);
    CLONESANTA_DRAWPINETREE3(585, screenHeight - 170, 0.58f);
    CLONESANTA_DRAWPINETREE3(764, screenHeight - 166, 0.55f);

    // 7. Three Village Houses Along the Upper Ridge
    // Left chalet
    CLONESANTA_DRAWHOLESOMEHOUSE3(
        82.0f, screenHeight - 166.0f, 52.0f, 30.0f, 24.0f,
        (Color){ 118, 68, 44, 255 }, true, time, 0.1f
    );

    // Center-right bakery with striped awning
    float bakX = 485.0f;
    float bakY = screenHeight - 168.0f;
    CLONESANTA_DRAWHOLESOMEHOUSE3(
        bakX, bakY, 68.0f, 34.0f, 26.0f,
        (Color){ 136, 76, 44, 255 }, true, time, 0.55f
    );
    int stripes = 6;
    float awningW = 32.0f / stripes;
    for (int st = 0; st < stripes; st++) {
        Color stripeCol = (st % 2 == 0) ? (Color){ 195, 42, 48, 255 } : (Color){ 248, 248, 250, 255 };
        DrawRectangle(bakX + 10.0f + st * awningW, bakY + 7.0f, awningW + 0.5f, 6.0f, stripeCol);
    }

    // Right ridge cottage
    CLONESANTA_DRAWHOLESOMEHOUSE3(
        635.0f, screenHeight - 164.0f, 50.0f, 30.0f, 22.0f,
        (Color){ 106, 64, 52, 255 }, false, time, 0.8f
    );

    // 8. Towering Foreground Pines (larger scale near bottom to emphasize looking up)
    CLONESANTA_DRAWPINETREE3(38,  screenHeight - 76, 1.35f);
    CLONESANTA_DRAWPINETREE3(148, screenHeight - 68, 1.05f);
    CLONESANTA_DRAWPINETREE3(265, screenHeight - 74, 0.88f);
    CLONESANTA_DRAWPINETREE3(545, screenHeight - 74, 0.90f);
    CLONESANTA_DRAWPINETREE3(668, screenHeight - 70, 1.12f);
    CLONESANTA_DRAWPINETREE3(766, screenHeight - 78, 1.40f);

    // 9. Low-Angle Foreground Snow Bank & Perspective Fence Posts
    DrawRectangle(0, screenHeight - 72, screenWidth, 72, (Color){ 238, 246, 255, 255 });

    // Perspective wooden fence converging slightly from bottom corners toward mid-ground
    Color postCol = { 86, 52, 32, 255 };
    DrawLineEx((Vector2){ 0.0f, screenHeight - 54.0f }, (Vector2){ screenWidth * 0.36f, screenHeight - 96.0f }, 3.0f, postCol);
    DrawLineEx((Vector2){ 0.0f, screenHeight - 40.0f }, (Vector2){ screenWidth * 0.36f, screenHeight - 86.0f }, 2.5f, postCol);
    DrawLineEx((Vector2){ screenWidth, screenHeight - 54.0f }, (Vector2){ screenWidth * 0.64f, screenHeight - 96.0f }, 3.0f, postCol);
    DrawLineEx((Vector2){ screenWidth, screenHeight - 40.0f }, (Vector2){ screenWidth * 0.64f, screenHeight - 86.0f }, 2.5f, postCol);

    for (int p = 0; p < 4; p++) {
        float t = p / 3.0f;
        float lx = t * (screenWidth * 0.34f);
        float ly = (screenHeight - 58.0f) - t * 38.0f;
        float ls = 1.0f - t * 0.45f;
        DrawRectangle(lx - 3.0f * ls, ly, 6.0f * ls, 24.0f * ls, postCol);
        DrawRectangle(lx - 4.0f * ls, ly - 2.0f * ls, 8.0f * ls, 4.0f * ls, (Color){ 242, 249, 255, 255 });

        float rx = screenWidth - t * (screenWidth * 0.34f);
        float ry = (screenHeight - 58.0f) - t * 38.0f;
        DrawRectangle(rx - 3.0f * ls, ry, 6.0f * ls, 24.0f * ls, postCol);
        DrawRectangle(rx - 4.0f * ls, ry - 2.0f * ls, 8.0f * ls, 4.0f * ls, (Color){ 242, 249, 255, 255 });
    }

    // 10. Festive Lantern & Icicle Garland Along Top Edge
    int nodeCount = 16;
    float spacing = (float)screenWidth / (nodeCount - 1);
    Color lanternColors[] = {
        (Color){ 255, 90, 95, 255 },
        (Color){ 255, 210, 80, 255 },
        (Color){ 95, 225, 175, 255 },
        (Color){ 130, 190, 255, 255 }
    };

    for (int i = 0; i < nodeCount; i++) {
        float lx  = i * spacing;
        float sag = sinf((float)i / (nodeCount - 1) * PI) * 24.0f;
        float ly  = 10.0f + sag;

        if (i < nodeCount - 1) {
            float nextLx  = (i + 1) * spacing;
            float nextSag = sinf((float)(i + 1) / (nodeCount - 1) * PI) * 24.0f;
            DrawLineEx((Vector2){ lx, ly }, (Vector2){ nextLx, 10.0f + nextSag }, 2.0f, (Color){ 45, 58, 78, 255 });
        }

        if (i % 2 == 1) {
            DrawTriangle(
                (Vector2){ lx - 3.0f, ly },
                (Vector2){ lx + 3.0f, ly },
                (Vector2){ lx, ly + 12.0f },
                Fade((Color){ 185, 230, 255, 255 }, 0.75f)
            );
        }

        Color col = lanternColors[i % 4];
        bool lit = ((int)(time * 2.0f) + i) % 2 == 0;
        if (lit) {
            DrawCircleV((Vector2){ lx, ly + 5.0f }, 10.0f, Fade(col, 0.28f));
        }
        DrawCircleV((Vector2){ lx, ly + 5.0f }, 4.5f, lit ? col : Fade(col, 0.40f));
    }

    // 11. Multi-Layered Swaying Snowfall
    for (int i = 0; i < 105; i++) {
        int layer    = i % 3;
        float speed  = 30.0f + layer * 26.0f;
        float radius = 1.4f + layer * 1.15f;
        float alpha  = 0.40f + layer * 0.24f;

        float baseX = (float)((i * 109) % screenWidth);
        float sway  = sinf(time * (1.1f + layer * 0.45f) + i * 0.8f) * (14.0f + layer * 6.0f);
        float snowX = fmodf(baseX + sway + screenWidth, (float)screenWidth);
        float snowY = fmodf((float)(i * 73) + time * speed, (float)screenHeight);

        DrawCircleV((Vector2){ snowX, snowY }, radius, Fade(WHITE, alpha));
    }
}


// Helper function to draw a layered pine tree with snow caps & warm lights
void CLONESANTA_DRAWPINETREE4(float x, float y, float scale)
{
    Color trunkBrown = { 82, 48, 28, 255 };
    Color darkPine   = { 14, 58, 48, 255 };
    Color midPine    = { 24, 84, 66, 255 };
    Color snowCap    = { 235, 246, 255, 255 };
    Color ornament   = { 255, 195, 75, 255 };

    // Trunk
    DrawRectangle(x - 10 * scale, y - 30 * scale, 20 * scale, 35 * scale, trunkBrown);

    // 3 Overlapping Foliage Tiers
    for (int i = 0; i < 3; i++) {
        float tierY  = y - (25 + i * 32) * scale;
        float width  = (55 - i * 10) * scale;
        float height = 50 * scale;

        DrawTriangle(
            (Vector2){ x - width, tierY },
            (Vector2){ x + width, tierY },
            (Vector2){ x, tierY - height },
            (i % 2 == 0) ? darkPine : midPine
        );

        DrawTriangle(
            (Vector2){ x - width * 0.35f, tierY - height * 0.65f },
            (Vector2){ x + width * 0.35f, tierY - height * 0.65f },
            (Vector2){ x, tierY - height },
            snowCap
        );

        DrawCircleV((Vector2){ x - width * 0.45f, tierY - height * 0.25f }, 2.4f * scale, ornament);
        DrawCircleV((Vector2){ x + width * 0.45f, tierY - height * 0.25f }, 2.4f * scale, ornament);
    }

    DrawCircleV((Vector2){ x, y - (25 + 2 * 32 + 52) * scale }, 3.2f * scale, Fade(ornament, 0.85f));
}

// Helper to draw a snow-roofed holiday house perched along the upper ridge
void CLONESANTA_DRAWHOLESOMEHOUSE4(
    float x,
    float y,
    float w,
    float h,
    float roofH,
    Color wallColor,
    bool hasChimney,
    float time,
    float phase
)
{
    if (hasChimney) {
        float chimX = x + w * 0.72f;
        float chimY = y - roofH * 0.65f;
        for (int s = 0; s < 4; s++) {
            float cycle  = fmodf(time * 0.35f + s * 0.25f + phase, 1.0f);
            float smokeX = chimX + 5.0f + sinf(time * 1.8f + s + phase * 3.0f) * 7.0f + cycle * 14.0f;
            float smokeY = (chimY - 16.0f) - cycle * 45.0f;
            DrawCircleV((Vector2){ smokeX, smokeY }, 3.8f + cycle * 6.5f, Fade((Color){ 235, 245, 255, 255 }, (1.0f - cycle) * 0.40f));
        }
        DrawRectangle(chimX, chimY - 14.0f, 9.0f, 18.0f, (Color){ 135, 52, 46, 255 });
        DrawRectangle(chimX - 1.5f, chimY - 16.0f, 12.0f, 4.0f, (Color){ 244, 250, 255, 255 });
    }

    // Main walls
    DrawRectangle(x, y, w, h, wallColor);

    // Pitched roof + snow layer
    DrawTriangle(
        (Vector2){ x - w * 0.12f, y },
        (Vector2){ x + w * 1.12f, y },
        (Vector2){ x + w * 0.5f, y - roofH },
        (Color){ 72, 44, 34, 255 }
    );
    DrawTriangle(
        (Vector2){ x - w * 0.10f, y - roofH * 0.12f },
        (Vector2){ x + w * 1.10f, y - roofH * 0.12f },
        (Vector2){ x + w * 0.5f, y - roofH },
        (Color){ 242, 249, 255, 255 }
    );

    // Warm flickering window
    float glow = 0.82f + 0.18f * sinf(time * 5.0f + phase * 4.0f);
    float winX = x + w * 0.18f;
    float winY = y + h * 0.30f;
    float winW = w * 0.28f;
    float winH = h * 0.36f;
    DrawRectangle(winX, winY, winW, winH, Fade((Color){ 255, 196, 68, 255 }, glow));
    DrawLine(winX + winW * 0.5f, winY, winX + winW * 0.5f, winY + winH, (Color){ 75, 42, 24, 255 });
    DrawLine(winX, winY + winH * 0.5f, winX + winW, winY + winH * 0.5f, (Color){ 75, 42, 24, 255 });

    // Door
    DrawRectangle(x + w * 0.60f, y + h * 0.36f, w * 0.22f, h * 0.64f, (Color){ 72, 38, 22, 255 });
}

// Low-Angle Perspective: Looking Up the Snowy Slope Toward Ridge Houses, Tall Pines & Aurora
void CLONESANTA_DRAWBACKGROUND4(int screenWidth, int screenHeight, float time)
{
    // 1. Deep Arctic Twilight Gradient (more sky visible from low angle)
    Color skyTop    = { 6, 12, 26, 255 };
    Color skyBottom = { 24, 50, 80, 255 };
    DrawRectangleGradientV(0, 0, screenWidth, screenHeight, skyTop, skyBottom);

    // 2. High-Arching Aurora Ribbons Across the Upper Sky
    for (int band = 0; band < 3; band++) {
        float baseY = screenHeight * (0.10f + band * 0.05f);
        Color auroraCol = (band == 0)
            ? (Color){ 65, 245, 175, 255 }
            : (band == 1)
                ? (Color){ 70, 190, 255, 255 }
                : (Color){ 175, 110, 255, 255 };

        for (int x = 0; x < screenWidth; x += 12) {
            float arch  = -sinf(((float)x / screenWidth) * PI) * 22.0f; // Arches upward toward center
            float wave1 = sinf(x * 0.012f + time * (0.9f + band * 0.25f) + band * 2.0f) * 15.0f;
            float wave2 = cosf(x * 0.025f - time * 0.6f) * 6.0f;
            float ay    = baseY + arch + wave1 + wave2;
            float pulse = 0.08f + 0.07f * sinf(time * 1.6f + x * 0.02f + band);

            DrawCircleV((Vector2){ (float)x, ay }, 26.0f, Fade(auroraCol, pulse));
            DrawCircleV((Vector2){ (float)x, ay + 14.0f }, 18.0f, Fade(auroraCol, pulse * 0.6f));
        }
    }

    // 3. Crisp Twinkling Stars
    for (int i = 0; i < 55; i++) {
        float sx = (float)((i * 151) % screenWidth);
        float sy = (float)((i * 83) % (int)(screenHeight * 0.44f));
        float twinkle = 0.35f + 0.65f * sinf(time * 3.0f + i * 1.9f);
        float radius = (i % 4 == 0) ? 2.1f : 1.2f;
        DrawCircleV((Vector2){ sx, sy }, radius, Fade(RAYWHITE, twinkle));
    }

    // 4. Crescent Winter Moon High in the Sky
    Vector2 moonPos = { screenWidth * 0.52f, 70.0f };
    DrawCircleV(moonPos, 42, Fade((Color){ 210, 245, 255, 255 }, 0.10f));
    DrawCircleV(moonPos, 31, Fade((Color){ 210, 245, 255, 255 }, 0.22f));
    DrawCircleV(moonPos, 23, (Color){ 238, 250, 255, 255 });
    DrawCircleV((Vector2){ moonPos.x + 9.0f, moonPos.y - 5.0f }, 20, skyTop);

    // 5. Distant Mountain Ridge Lower in Frame (camera tilted upward)
    Color peakFar   = { 34, 56, 86, 255 };
    Color hillUpper = { 166, 196, 224, 255 };
    Color hillLower = { 216, 236, 252, 255 };

    DrawTriangle((Vector2){ 15,  screenHeight - 135 }, (Vector2){ 285, screenHeight - 135 }, (Vector2){ 150, screenHeight - 235 }, peakFar);
    DrawTriangle((Vector2){ 240, screenHeight - 135 }, (Vector2){ 550, screenHeight - 135 }, (Vector2){ 395, screenHeight - 255 }, peakFar);
    DrawTriangle((Vector2){ 500, screenHeight - 135 }, (Vector2){ 785, screenHeight - 135 }, (Vector2){ 642, screenHeight - 230 }, peakFar);

    // Rising hillside sweeping up from lower ground to upper ridge
    DrawCircle(180, screenHeight + 90, 360, hillUpper);
    DrawCircle(620, screenHeight + 95, 360, hillUpper);
    DrawCircle(400, screenHeight + 175, 430, hillLower);

    // 6. Upper Ridge Pine Trees (smaller, sitting along the crest)
    CLONESANTA_DRAWPINETREE4(36,  screenHeight - 168, 0.55f);
    CLONESANTA_DRAWPINETREE4(225, screenHeight - 172, 0.58f);
    CLONESANTA_DRAWPINETREE4(345, screenHeight - 162, 0.52f);
    CLONESANTA_DRAWPINETREE4(465, screenHeight - 164, 0.54f);
    CLONESANTA_DRAWPINETREE4(585, screenHeight - 170, 0.58f);
    CLONESANTA_DRAWPINETREE4(764, screenHeight - 166, 0.55f);

    // 7. Three Village Houses Along the Upper Ridge
    // Left chalet
    CLONESANTA_DRAWHOLESOMEHOUSE4(
        82.0f, screenHeight - 166.0f, 52.0f, 30.0f, 24.0f,
        (Color){ 118, 68, 44, 255 }, true, time, 0.1f
    );

    // Center-right bakery with striped awning
    float bakX = 485.0f;
    float bakY = screenHeight - 168.0f;
    CLONESANTA_DRAWHOLESOMEHOUSE4(
        bakX, bakY, 68.0f, 34.0f, 26.0f,
        (Color){ 136, 76, 44, 255 }, true, time, 0.55f
    );
    int stripes = 6;
    float awningW = 32.0f / stripes;
    for (int st = 0; st < stripes; st++) {
        Color stripeCol = (st % 2 == 0) ? (Color){ 195, 42, 48, 255 } : (Color){ 248, 248, 250, 255 };
        DrawRectangle(bakX + 10.0f + st * awningW, bakY + 7.0f, awningW + 0.5f, 6.0f, stripeCol);
    }

    // Right ridge cottage
    CLONESANTA_DRAWHOLESOMEHOUSE4(
        635.0f, screenHeight - 164.0f, 50.0f, 30.0f, 22.0f,
        (Color){ 106, 64, 52, 255 }, false, time, 0.8f
    );

    // 8. Towering Foreground Pines (larger scale near bottom to emphasize looking up)
    CLONESANTA_DRAWPINETREE4(38,  screenHeight - 76, 1.35f);
    CLONESANTA_DRAWPINETREE4(148, screenHeight - 68, 1.05f);
    CLONESANTA_DRAWPINETREE4(265, screenHeight - 74, 0.88f);
    CLONESANTA_DRAWPINETREE4(545, screenHeight - 74, 0.90f);
    CLONESANTA_DRAWPINETREE4(668, screenHeight - 70, 1.12f);
    CLONESANTA_DRAWPINETREE4(766, screenHeight - 78, 1.40f);

    // 9. Low-Angle Foreground Snow Bank & Perspective Fence Posts
    DrawRectangle(0, screenHeight - 72, screenWidth, 72, (Color){ 238, 246, 255, 255 });

    // Perspective wooden fence converging slightly from bottom corners toward mid-ground
    Color postCol = { 86, 52, 32, 255 };
    DrawLineEx((Vector2){ 0.0f, screenHeight - 54.0f }, (Vector2){ screenWidth * 0.36f, screenHeight - 96.0f }, 3.0f, postCol);
    DrawLineEx((Vector2){ 0.0f, screenHeight - 40.0f }, (Vector2){ screenWidth * 0.36f, screenHeight - 86.0f }, 2.5f, postCol);
    DrawLineEx((Vector2){ screenWidth, screenHeight - 54.0f }, (Vector2){ screenWidth * 0.64f, screenHeight - 96.0f }, 3.0f, postCol);
    DrawLineEx((Vector2){ screenWidth, screenHeight - 40.0f }, (Vector2){ screenWidth * 0.64f, screenHeight - 86.0f }, 2.5f, postCol);

    for (int p = 0; p < 4; p++) {
        float t = p / 3.0f;
        float lx = t * (screenWidth * 0.34f);
        float ly = (screenHeight - 58.0f) - t * 38.0f;
        float ls = 1.0f - t * 0.45f;
        DrawRectangle(lx - 3.0f * ls, ly, 6.0f * ls, 24.0f * ls, postCol);
        DrawRectangle(lx - 4.0f * ls, ly - 2.0f * ls, 8.0f * ls, 4.0f * ls, (Color){ 242, 249, 255, 255 });

        float rx = screenWidth - t * (screenWidth * 0.34f);
        float ry = (screenHeight - 58.0f) - t * 38.0f;
        DrawRectangle(rx - 3.0f * ls, ry, 6.0f * ls, 24.0f * ls, postCol);
        DrawRectangle(rx - 4.0f * ls, ry - 2.0f * ls, 8.0f * ls, 4.0f * ls, (Color){ 242, 249, 255, 255 });
    }

    // 10. Festive Lantern & Icicle Garland Along Top Edge
    int nodeCount = 16;
    float spacing = (float)screenWidth / (nodeCount - 1);
    Color lanternColors[] = {
        (Color){ 255, 90, 95, 255 },
        (Color){ 255, 210, 80, 255 },
        (Color){ 95, 225, 175, 255 },
        (Color){ 130, 190, 255, 255 }
    };

    for (int i = 0; i < nodeCount; i++) {
        float lx  = i * spacing;
        float sag = sinf((float)i / (nodeCount - 1) * PI) * 24.0f;
        float ly  = 10.0f + sag;

        if (i < nodeCount - 1) {
            float nextLx  = (i + 1) * spacing;
            float nextSag = sinf((float)(i + 1) / (nodeCount - 1) * PI) * 24.0f;
            DrawLineEx((Vector2){ lx, ly }, (Vector2){ nextLx, 10.0f + nextSag }, 2.0f, (Color){ 45, 58, 78, 255 });
        }

        if (i % 2 == 1) {
            DrawTriangle(
                (Vector2){ lx - 3.0f, ly },
                (Vector2){ lx + 3.0f, ly },
                (Vector2){ lx, ly + 12.0f },
                Fade((Color){ 185, 230, 255, 255 }, 0.75f)
            );
        }

        Color col = lanternColors[i % 4];
        bool lit = ((int)(time * 2.0f) + i) % 2 == 0;
        if (lit) {
            DrawCircleV((Vector2){ lx, ly + 5.0f }, 10.0f, Fade(col, 0.28f));
        }
        DrawCircleV((Vector2){ lx, ly + 5.0f }, 4.5f, lit ? col : Fade(col, 0.40f));
    }

    // 11. Multi-Layered Swaying Snowfall
    for (int i = 0; i < 105; i++) {
        int layer    = i % 3;
        float speed  = 30.0f + layer * 26.0f;
        float radius = 1.4f + layer * 1.15f;
        float alpha  = 0.40f + layer * 0.24f;

        float baseX = (float)((i * 109) % screenWidth);
        float sway  = sinf(time * (1.1f + layer * 0.45f) + i * 0.8f) * (14.0f + layer * 6.0f);
        float snowX = fmodf(baseX + sway + screenWidth, (float)screenWidth);
        float snowY = fmodf((float)(i * 73) + time * speed, (float)screenHeight);

        DrawCircleV((Vector2){ snowX, snowY }, radius, Fade(WHITE, alpha));
    }
}

// Zeer gedetailleerde en schaalbare Kerstman
void CLONESANTA_DRAWCLONE(Vector2 center, float s, VOWEL mouthVowel) 
{
    float x = center.x;
    float y = center.y;

    // Kleurenpalet
    Color santaRed      = { 212, 34, 38, 255 };
    Color darkRed       = { 155, 20, 24, 255 };
    Color highlightRed  = { 235, 62, 65, 255 };
    Color skinColor     = { 255, 213, 184, 255 };
    Color skinShadow    = { 232, 178, 148, 255 };
    Color noseColor     = { 242, 122, 118, 255 };
    Color noseShadow    = { 205, 88, 86, 255 };
    Color cheekColor    = { 248, 160, 155, 255 };
    Color beardWhite    = { 246, 248, 252, 255 };
    Color beardShadow   = { 214, 222, 235, 255 };
    Color trimWhite     = { 255, 255, 255, 255 };
    Color mouthDark     = { 65, 14, 20, 255 };
    Color mouthDeep     = { 40, 8, 12, 255 };
    Color tongueColor   = { 220, 88, 98, 255 };
    Color goldColor     = { 245, 198, 66, 255 };
    Color goldShadow    = { 195, 145, 30, 255 };
    Color beltBlack     = { 32, 34, 40, 255 };
    Color irisBlue      = { 65, 125, 175, 255 };

    // 1. JAS, SCHOUDERS & ARMEN
    // Achterste schouderpartij
    DrawEllipse(x, y + 175 * s, 128 * s, 105 * s, darkRed);
    DrawEllipse(x, y + 172 * s, 122 * s, 100 * s, santaRed);

    // Subtiele plooien/schaduwen op de jas
    DrawEllipse(x - 65 * s, y + 165 * s, 32 * s, 65 * s, highlightRed);
    DrawEllipse(x + 65 * s, y + 165 * s, 32 * s, 65 * s, highlightRed);
    DrawRing((Vector2){ x - 75 * s, y + 160 * s }, 35 * s, 40 * s, 140, 240, 16, darkRed);
    DrawRing((Vector2){ x + 75 * s, y + 160 * s }, 35 * s, 40 * s, -60, 40, 16, darkRed);

    // Verticale witte bontstrip in het midden van de jas
    DrawRectangle(x - 18 * s, y + 75 * s, 36 * s, 140 * s, beardShadow);
    DrawRectangle(x - 16 * s, y + 75 * s, 32 * s, 140 * s, trimWhite);

    // Gouden knopen op de bontstrip
    float buttonOffsets[] = { 112.0f, 142.0f, 198.0f };
    for (int i = 0; i < 3; i++) {
        float by = y + buttonOffsets[i] * s;
        DrawCircle(x, by + 1.5f * s, 5.5f * s, beardShadow);
        DrawCircle(x, by, 5.5f * s, goldShadow);
        DrawCircle(x, by - 0.5f * s, 4.5f * s, goldColor);
        // Knoopsgaatjes
        DrawCircle(x - 1.5f * s, by - 1.5f * s, 0.8f * s, goldShadow);
        DrawCircle(x + 1.5f * s, by - 1.5f * s, 0.8f * s, goldShadow);
        DrawCircle(x - 1.5f * s, by + 1.5f * s, 0.8f * s, goldShadow);
        DrawCircle(x + 1.5f * s, by + 1.5f * s, 0.8f * s, goldShadow);
    }

    // Zwarte riem met gouden gesp
    DrawRectangle(x - 115 * s, y + 162 * s, 230 * s, 22 * s, beltBlack);
    DrawRectangleLinesEx((Rectangle){ x - 115 * s, y + 162 * s, 230 * s, 22 * s }, 1.5f * s, BLACK);
    // Riemgesp
    DrawRectangleRounded((Rectangle){ x - 22 * s, y + 158 * s, 44 * s, 30 * s }, 0.25f, 8, goldShadow);
    DrawRectangleRounded((Rectangle){ x - 20 * s, y + 159 * s, 40 * s, 28 * s }, 0.25f, 8, goldColor);
    DrawRectangleRounded((Rectangle){ x - 12 * s, y + 164 * s, 24 * s, 18 * s }, 0.15f, 6, beltBlack);
    DrawRectangle(x - 12 * s, y + 171.5f * s, 14 * s, 3 * s, goldColor); // Gesp-pin

    // 2. GELAAGDE BAARD (Achtergrond & Krullen)
    // Schaduw van de baard op de jas
    DrawCircle(x, y + 48 * s, 86 * s, Fade(BLACK, 0.15f));
    DrawCircle(x, y + 86 * s, 68 * s, Fade(BLACK, 0.15f));

    // Donkere onderlaag van de baard voor diepte
    DrawCircle(x, y + 42 * s, 84 * s, beardShadow);
    DrawCircle(x - 50 * s, y + 48 * s, 52 * s, beardShadow);
    DrawCircle(x + 50 * s, y + 48 * s, 52 * s, beardShadow);
    DrawCircle(x - 32 * s, y + 76 * s, 48 * s, beardShadow);
    DrawCircle(x + 32 * s, y + 76 * s, 48 * s, beardShadow);
    DrawCircle(x, y + 88 * s, 56 * s, beardShadow);

    // Heldere voorgrondplukken van de baard
    DrawCircle(x, y + 36 * s, 80 * s, beardWhite);
    DrawCircle(x - 48 * s, y + 42 * s, 48 * s, beardWhite);
    DrawCircle(x + 48 * s, y + 42 * s, 48 * s, beardWhite);
    DrawCircle(x - 30 * s, y + 70 * s, 44 * s, beardWhite);
    DrawCircle(x + 30 * s, y + 70 * s, 44 * s, beardWhite);
    DrawCircle(x, y + 82 * s, 50 * s, beardWhite);

    // Extra baardkrullen onderaan voor textuur
    DrawCircle(x - 18 * s, y + 102 * s, 22 * s, beardWhite);
    DrawCircle(x + 18 * s, y + 102 * s, 22 * s, beardWhite);
    DrawCircle(x, y + 110 * s, 24 * s, beardWhite);

    // 3. OREN & GEZICHT
    // Oren (met binnenoor-schaduw)
    DrawCircle(x - 62 * s, y - 6 * s, 13 * s, skinColor);
    DrawCircle(x - 62 * s, y - 6 * s, 7 * s, skinShadow);
    DrawCircle(x + 62 * s, y - 6 * s, 13 * s, skinColor);
    DrawCircle(x + 62 * s, y - 6 * s, 7 * s, skinShadow);

    // Hoofd / Gezichtsvorm
    DrawCircle(x, y - 10 * s, 62 * s, skinColor);
    // Subtiele schaduw onder de mutsrand op het voorhoofd
    DrawEllipse(x, y - 36 * s, 56 * s, 14 * s, skinShadow);

    // 4. KERSTMUTS MET PLOOIEN & BONT
    // Achterste omgevouwen punt van de muts
    DrawTriangle(
        (Vector2){ x + 42 * s, y - 65 * s },
        (Vector2){ x + 98 * s, y - 18 * s },
        (Vector2){ x + 72 * s, y - 78 * s },
        darkRed
    );
    // Donzige pompoen met schaduw
    DrawCircle(x + 98 * s, y - 12 * s, 19 * s, beardShadow);
    DrawCircle(x + 97 * s, y - 14 * s, 18 * s, trimWhite);
    DrawCircle(x + 92 * s, y - 19 * s, 9 * s, trimWhite);

    // Hoofdkegel van de muts
    DrawTriangle(
        (Vector2){ x - 68 * s, y - 48 * s },
        (Vector2){ x + 68 * s, y - 48 * s },
        (Vector2){ x + 32 * s, y - 148 * s },
        santaRed
    );
    // Lichtaccent & stofplooi op de muts
    DrawTriangle(
        (Vector2){ x - 35 * s, y - 55 * s },
        (Vector2){ x + 5 * s, y - 55 * s },
        (Vector2){ x + 22 * s, y - 135 * s },
        highlightRed
    );
    DrawLineEx((Vector2){ x + 15 * s, y - 65 * s }, (Vector2){ x + 48 * s, y - 82 * s }, 3.0f * s, darkRed);

    // Bontrand van de muts (opgebouwd uit pluizige bolletjes i.p.v. een strakke balk)
    for (int i = -5; i <= 5; i++) {
        float bx = x + i * 13.0f * s;
        float by = y - 50.0f * s + (i % 2 == 0 ? 1.5f * s : -1.5f * s);
        DrawCircle(bx, by + 2 * s, 15 * s, beardShadow);
        DrawCircle(bx, by, 15 * s, trimWhite);
    }

    // 5. OGEN, WENKBRAUWEN & BRIL
    // Lachrimpeltjes naast de ogen
    DrawRing((Vector2){ x - 38 * s, y - 16 * s }, 7 * s, 8.5f * s, 140, 220, 12, skinShadow);
    DrawRing((Vector2){ x + 38 * s, y - 16 * s }, 7 * s, 8.5f * s, -40, 40, 12, skinShadow);

    // Oogwit, iris, pupil en dubbele glans
    float eyeOffsets[] = { -24.0f, 24.0f };
    for (int i = 0; i < 2; i++) {
        float ex = x + eyeOffsets[i] * s;
        float ey = y - 17.0f * s;
        DrawCircle(ex, ey, 8.5f * s, WHITE);
        DrawCircle(ex, ey, 5.5f * s, irisBlue);
        DrawCircle(ex, ey, 3.0f * s, BLACK);
        DrawCircle(ex - 2.0f * s, ey - 2.2f * s, 1.8f * s, WHITE); // Hoofdglans
        DrawCircle(ex + 2.0f * s, ey + 1.8f * s, 0.9f * s, WHITE); // Secundaire glans
    }

    // Ronde gouden leesbril
    DrawRing((Vector2){ x - 24 * s, y - 15 * s }, 13.5f * s, 15.2f * s, 0, 360, 32, goldShadow);
    DrawRing((Vector2){ x + 24 * s, y - 15 * s }, 13.5f * s, 15.2f * s, 0, 360, 32, goldShadow);
    // Neusbrug & pootjes van de bril
    DrawRing((Vector2){ x, y - 14 * s }, 9.0f * s, 10.8f * s, 200, 340, 16, goldShadow);
    DrawLineEx((Vector2){ x - 39 * s, y - 15 * s }, (Vector2){ x - 58 * s, y - 12 * s }, 1.8f * s, goldShadow);
    DrawLineEx((Vector2){ x + 39 * s, y - 15 * s }, (Vector2){ x + 58 * s, y - 12 * s }, 1.8f * s, goldShadow);
    // Subtiele glasreflectie op de bril
    DrawLineEx((Vector2){ x - 31 * s, y - 21 * s }, (Vector2){ x - 25 * s, y - 15 * s }, 1.5f * s, Fade(WHITE, 0.5f));
    DrawLineEx((Vector2){ x + 17 * s, y - 21 * s }, (Vector2){ x + 23 * s, y - 15 * s }, 1.5f * s, Fade(WHITE, 0.5f));

    // Borstelige witte wenkbrauwen (gelaagd)
    DrawEllipse(x - 25 * s, y - 33 * s, 15 * s, 5.5f * s, beardShadow);
    DrawEllipse(x - 25 * s, y - 34 * s, 15 * s, 5.0f * s, trimWhite);
    DrawEllipse(x + 25 * s, y - 33 * s, 15 * s, 5.5f * s, beardShadow);
    DrawEllipse(x + 25 * s, y - 34 * s, 15 * s, 5.0f * s, trimWhite);

    // 6. RODE WANGEN MET GLANS
    DrawCircle(x - 40 * s, y + 2 * s, 12 * s, cheekColor);
    DrawCircle(x + 40 * s, y + 2 * s, 12 * s, cheekColor);
    DrawEllipse(x - 42 * s, y - 1 * s, 4 * s, 2.5f * s, Fade(WHITE, 0.45f));
    DrawEllipse(x + 42 * s, y - 1 * s, 4 * s, 2.5f * s, Fade(WHITE, 0.45f));

    // 7. MOND (Klinkers met tanden, tong en keeldiepte)
    float mouthY = y + 24 * s;
    switch (mouthVowel) {
        case VOWEL_NONE:
            // Vriendelijke gesloten glimlach + onderlip-schaduw
            DrawRing((Vector2){ x, mouthY - 5 * s }, 12 * s, 15 * s, 35, 145, 24, mouthDark);
            DrawEllipse(x, mouthY + 13 * s, 8 * s, 2.5f * s, skinShadow);
            break;

        case VOWEL_A:
            // Grote open "Ah"-mond
            DrawEllipse(x, mouthY + 8 * s, 21 * s, 26 * s, mouthDark);
            DrawEllipse(x, mouthY + 4 * s, 14 * s, 16 * s, mouthDeep); // Keeldiepte
            // Boventanden
            DrawRectangleRounded((Rectangle){ x - 13 * s, mouthY - 12 * s, 26 * s, 6 * s }, 0.5f, 6, WHITE);
            // Tong met middengroef
            DrawEllipse(x, mouthY + 22 * s, 14 * s, 9 * s, tongueColor);
            DrawLineEx((Vector2){ x, mouthY + 15 * s }, (Vector2){ x, mouthY + 26 * s }, 1.5f * s, mouthDark);
            break;

        case VOWEL_E:
            // Brede "Eh / Ee"-mond
            DrawEllipse(x, mouthY + 3 * s, 27 * s, 12 * s, mouthDark);
            // Boven- en ondertanden
            DrawRectangleRounded((Rectangle){ x - 18 * s, mouthY - 6 * s, 36 * s, 5 * s }, 0.4f, 6, WHITE);
            DrawRectangleRounded((Rectangle){ x - 15 * s, mouthY + 7 * s, 30 * s, 4 * s }, 0.4f, 6, beardShadow);
            // Tong in het midden
            DrawEllipse(x, mouthY + 4 * s, 14 * s, 4 * s, tongueColor);
            break;

        case VOWEL_O:
            // Ronde "Oh / Ho"-mond
            DrawCircle(x, mouthY + 6 * s, 14 * s, mouthDark);
            DrawCircle(x, mouthY + 5 * s, 9 * s, mouthDeep);
            DrawEllipse(x, mouthY + 14 * s, 7 * s, 4 * s, tongueColor);
            break;
    }

    // 8. VOLLE, GELAAGDE SNOR
    // Schaduw onder de snor
    DrawCircle(x - 21 * s, y + 17 * s, 21 * s, beardShadow);
    DrawCircle(x + 21 * s, y + 17 * s, 21 * s, beardShadow);
    DrawEllipse(x - 37 * s, y + 20 * s, 16 * s, 10 * s, beardShadow);
    DrawEllipse(x + 37 * s, y + 20 * s, 16 * s, 10 * s, beardShadow);

    // Witte voorgrond van de snor
    DrawCircle(x - 21 * s, y + 14 * s, 20 * s, trimWhite);
    DrawCircle(x + 21 * s, y + 14 * s, 20 * s, trimWhite);
    DrawEllipse(x - 37 * s, y + 17 * s, 16 * s, 10 * s, trimWhite);
    DrawEllipse(x + 37 * s, y + 17 * s, 16 * s, 10 * s, trimWhite);
    // Scheidingslijn in het midden van de snor
    DrawLineEx((Vector2){ x, y + 4 * s }, (Vector2){ x, y + 24 * s }, 1.5f * s, beardShadow);

    // 9. NEUS MET SCHADUW & GLANS
    // Neusschaduw op de snor
    DrawEllipse(x, y + 5 * s, 15 * s, 11 * s, beardShadow);
    // Neus zelf
    DrawEllipse(x, y + 2 * s, 15 * s, 11.5f * s, noseShadow);
    DrawEllipse(x, y + 1 * s, 14.5f * s, 10.5f * s, noseColor);
    // Glansplekjes op de neus
    DrawEllipse(x - 4.5f * s, y - 2.5f * s, 4.5f * s, 2.8f * s, (Color){ 255, 195, 195, 255 });
    DrawCircle(x - 8 * s, y + 0.5f * s, 1.5f * s, (Color){ 255, 210, 210, 255 });
}

