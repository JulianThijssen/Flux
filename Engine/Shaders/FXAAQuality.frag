#version 150 core

in vec3 pass_position;
in vec2 pass_texCoords;

out vec4 fragColor;

uniform sampler2D tex;
uniform vec2 rcpScreenSize;

const float FXAA_EDGE_THRESHOLD = 1.0 / 8;
const float FXAA_EDGE_THRESHOLD_MIN = 1.0 / 16;
const float FXAA_QUALITY_SUBPIX = 0.75;

const vec3 toLuma = vec3(0.299, 0.587, 0.114);

const float FxaaQuality[5] = float[](1.0, 1.5, 2.0, 4.0, 12.0);

#define saturate(x) clamp(x, 0.0, 1.0);

void main()
{
    vec2 fxaaQualityRcpFrame = vec2(rcpScreenSize.x, rcpScreenSize.y);

    vec2 posM = vec2(pass_texCoords.x, pass_texCoords.y);

    float lumaN = dot(textureOffset(tex, posM, ivec2( 0, -1)).rgb, toLuma);
    float lumaW = dot(textureOffset(tex, posM, ivec2(-1,  0)).rgb, toLuma);
    float lumaM = dot(textureOffset(tex, posM, ivec2( 0,  0)).rgb, toLuma);
    float lumaE = dot(textureOffset(tex, posM, ivec2( 1,  0)).rgb, toLuma);
    float lumaS = dot(textureOffset(tex, posM, ivec2( 0,  1)).rgb, toLuma);
    
    float rangeMin = min(lumaM, min(min(lumaN, lumaW), min(lumaS, lumaE)));
    float rangeMax = max(lumaM, max(max(lumaN, lumaW), max(lumaS, lumaE)));
    // Range of luma values surrounding this pixel
    float range = rangeMax - rangeMin;
    
    // Exit early if the range of luma values is not big enough
    if (range < max(FXAA_EDGE_THRESHOLD_MIN, rangeMax * FXAA_EDGE_THRESHOLD)) {
        fragColor = vec4(texture(tex, posM).rgb, 1);
        return;
    }
    
    // Calculate the rest of the 3x3 neighbourhood luma values
    float lumaNW = dot(textureOffset(tex, posM, ivec2(-1, -1)).rgb, toLuma);
    float lumaSE = dot(textureOffset(tex, posM, ivec2( 1,  1)).rgb, toLuma);
    float lumaNE = dot(textureOffset(tex, posM, ivec2( 1, -1)).rgb, toLuma);
    float lumaSW = dot(textureOffset(tex, posM, ivec2(-1,  1)).rgb, toLuma);

    // Vertical / Horizontal Edge Test
    float edgeVert =
        abs((1 * lumaNW) + (-2 * lumaN) + (1 * lumaNE)) +
        abs((2 * lumaW ) + (-4 * lumaM) + (2 * lumaE )) +
        abs((1 * lumaSW) + (-2 * lumaS) + (1 * lumaSE));
    float edgeHorz =
        abs((1 * lumaNW) + (-2 * lumaW) + (1 * lumaSW)) +
        abs((2 * lumaN ) + (-4 * lumaM) + (2 * lumaS )) +
        abs((1 * lumaNE) + (-2 * lumaE) + (1 * lumaSE));
    bool horzSpan = edgeHorz >= edgeVert;
    
    // Sub-pixel aliasing test
    // Average luma value (Lowpass luma)
    float lumaL = (lumaN + lumaW + lumaE + lumaS);
    float subpixRcpRange = 1.0 / range;
    float lengthSign = fxaaQualityRcpFrame.x;
    float subpixA = lumaL * 2.0 + (lumaNE + lumaNW + lumaSW + lumaSE);
    float subpixB = (subpixA * (1.0 / 12.0)) - lumaM;
    float subpixC = saturate(abs(subpixB) * subpixRcpRange);
    float subpixD = (-2.0 * subpixC) + 3.0;
    float subpixE = subpixC * subpixC;
    float subpixF = subpixD * subpixE;
    float subpixG = subpixF * subpixF;
    float subpixH = subpixG * FXAA_QUALITY_SUBPIX;
    
    // If we have a vertical edge, switch variables around
    if (!horzSpan) {
        lumaN = lumaW;
        lumaS = lumaE;
    }
    if (horzSpan)
        lengthSign = fxaaQualityRcpFrame.y;
    
    float gradientN = lumaN - lumaM;
    float gradientS = lumaS - lumaM;
    float lumaNN = lumaN + lumaM;
    float lumaSS = lumaS + lumaM;
    bool pairN = abs(gradientN) >= abs(gradientS);
    float gradient = max(abs(gradientN), abs(gradientS));
    if (pairN) lengthSign = -lengthSign;
/*--------------------------------------------------------------------------*/
    vec2 posB = vec2(posM.x, posM.y);
    vec2 offNP = vec2(0, 0);
    offNP.x = (!horzSpan) ? 0.0 : fxaaQualityRcpFrame.x;
    offNP.y = ( horzSpan) ? 0.0 : fxaaQualityRcpFrame.y;
    if (!horzSpan) posB.x += lengthSign * 0.5;
    if ( horzSpan) posB.y += lengthSign * 0.5;
/*--------------------------------------------------------------------------*/
    vec2 posN = vec2(posB.x - offNP.x * FxaaQuality[0], posB.y - offNP.y * FxaaQuality[0]);
    vec2 posP = vec2(posB.x + offNP.x * FxaaQuality[0], posB.y + offNP.y * FxaaQuality[0]);
    float lumaEndN = dot(texture(tex, posN).rgb, toLuma);
    float lumaEndP = dot(texture(tex, posP).rgb, toLuma);
/*--------------------------------------------------------------------------*/
    if (!pairN) lumaNN = lumaSS;
    float gradientScaled = gradient * 1.0 / 4;
    float lumaMM = lumaM - lumaNN * 0.5;
    bool lumaMLTZero = lumaMM < 0.0;
/*--------------------------------------------------------------------------*/
    lumaEndN -= lumaNN * 0.5;
    lumaEndP -= lumaNN * 0.5;
    bool doneN = abs(lumaEndN) >= gradientScaled;
    bool doneP = abs(lumaEndP) >= gradientScaled;
    if (!doneN) posN.x -= offNP.x * FxaaQuality[1];
    if (!doneN) posN.y -= offNP.y * FxaaQuality[1];
    bool doneNP = (!doneN) || (!doneP);
    if (!doneP) posP.x += offNP.x * FxaaQuality[1];
    if (!doneP) posP.y += offNP.y * FxaaQuality[1];
/*--------------------------------------------------------------------------*/
    for (int i = 2; i < FxaaQuality.length; i++) {
        if (doneNP) {
            if (!doneN) lumaEndN = dot(texture(tex, posN).rgb, toLuma);
            if (!doneP) lumaEndP = dot(texture(tex, posP).rgb, toLuma);
            if (!doneN) lumaEndN -= lumaNN * 0.5;
            if (!doneP) lumaEndP -= lumaNN * 0.5;
            doneN = abs(lumaEndN) >= gradientScaled;
            doneP = abs(lumaEndP) >= gradientScaled;
            if (!doneN) posN.x -= offNP.x * FxaaQuality[i];
            if (!doneN) posN.y -= offNP.y * FxaaQuality[i];
            doneNP = (!doneN) || (!doneP);
            if (!doneP) posP.x += offNP.x * FxaaQuality[i];
            if (!doneP) posP.y += offNP.y * FxaaQuality[i];
        }
    }

/*--------------------------------------------------------------------------*/
    float dstN = posM.x - posN.x;
    float dstP = posP.x - posM.x;
    if (!horzSpan) dstN = posM.y - posN.y;
    if (!horzSpan) dstP = posP.y - posM.y;
    
    bool goodSpanN = (lumaEndN < 0.0) != lumaMLTZero;
    float spanLength = (dstP + dstN);
    bool goodSpanP = (lumaEndP < 0.0) != lumaMLTZero;
    float spanLengthRcp = 1.0 / spanLength;
    
    bool directionN = dstN < dstP;
    float dst = min(dstN, dstP);
    bool goodSpan = directionN ? goodSpanN : goodSpanP;
    float pixelOffset = (dst * (-spanLengthRcp)) + 0.5;
    
    float pixelOffsetGood = goodSpan ? pixelOffset : 0.0;
    float pixelOffsetSubpix = max(pixelOffsetGood, subpixH);
    if (!horzSpan) posM.x += pixelOffsetSubpix * lengthSign;
    if ( horzSpan) posM.y += pixelOffsetSubpix * lengthSign;
    
    fragColor = vec4(texture(tex, posM).rgb, lumaM);
}
