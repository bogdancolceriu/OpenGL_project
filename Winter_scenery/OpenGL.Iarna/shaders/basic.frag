#version 410 core

in vec3 fragNormal;
in vec4 fragPosEye;
in vec2 texCoords;
in vec4 fragPosLightSpace;
in vec3 fragWorldPos;
in vec4 localPos;

out vec4 outputColor;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat3 uNormalMatrix;

uniform vec3 dirLightVector;
uniform vec3 mainLightColor;
uniform vec3 extraLightColorA;
uniform vec3 extraLightColorB;
uniform vec3 viewWorldPos;
uniform int fogToggle;
uniform vec3 pointPosA;
uniform vec3 pointPosB;
uniform vec3 pointPosC;
uniform vec3 pointPosD;
uniform vec3 pointPosE;

uniform sampler2D diffuseMap;
uniform sampler2D specularMap;
uniform sampler2D shadowAtlas;

float lightConst = 1.0;
float lightLinear = 0.07;
float lightQuadratic = 0.14;

float pointAmbientFactor = 0.5;
float pointSpecularStr = 0.5;
float pointShininess = 32.0;

vec3 dirAmbient;
float globalAmbientStr = 0.2;
vec3 dirDiffuse;
vec3 dirSpecular;
float dirSpecStr = 0.5;
float dirShiny = 32.0;

float resultShadow;

void evaluateDirectionalLight() 
{
    vec3 normalView = normalize(fragNormal);
    vec3 directionalRay = normalize(dirLightVector);
    vec3 eyePositionCam = vec3(0.0);

    vec3 eyeDir = normalize(eyePositionCam - fragPosEye.xyz);

    dirAmbient = globalAmbientStr * mainLightColor;
    dirDiffuse = max(dot(normalView, directionalRay), 0.0) * mainLightColor;

    vec3 reflectRay = reflect(-directionalRay, normalView);
    float specFactor = pow(max(dot(eyeDir, reflectRay), 0.0), dirShiny);
    dirSpecular = dirSpecStr * specFactor * mainLightColor;
}

vec3 evaluatePointLight(vec3 lightPos, vec3 lightColorVal)
{
    vec3 normalRes = normalize(uNormalMatrix * fragNormal);
    vec3 pathToLight = normalize(lightPos - localPos.xyz);
    float distLight = length(lightPos - localPos.xyz);

    vec3 camSpaceDir = vec3(0.0) - fragPosEye.xyz;
    vec3 viewDirNorm = normalize(camSpaceDir);
    vec3 halfVec = normalize(pathToLight + viewDirNorm);

    float attenuation = 1.0 / (lightConst + lightLinear * distLight + 
                               lightQuadratic * (distLight * distLight));

    vec3 ambTerm = pointAmbientFactor * lightColorVal;
    vec3 diffTerm = max(dot(normalRes, pathToLight), 0.0) * lightColorVal;
    float spCoeff = pow(max(dot(normalRes, halfVec), 0.0), pointShininess);
    vec3 specTerm = pointSpecularStr * spCoeff * lightColorVal;
    return (ambTerm + diffTerm + specTerm) * attenuation;
}

float evaluateShadowMap()
{
    vec3 coordNDC = fragPosLightSpace.xyz / fragPosLightSpace.w;
    coordNDC = coordNDC * 0.5 + 0.5;

    float closestDepth = texture(shadowAtlas, coordNDC.xy).r;
    float currentDepth = coordNDC.z;
    float offsetBias = 0.005;

    float shadowVal = currentDepth - offsetBias > closestDepth ? 1.0 : 0.0;
    if (coordNDC.z > 1.0) 
        return 0.0;
    return shadowVal;
}

float evaluateFogFactor()
{
    float densityVal = 0.01;
    float distFrag = length(fragPosEye);
    float exponentVal = exp(-pow(distFrag * densityVal, 2));
    return clamp(exponentVal, 0.0, 1.0);
}

void main() 
{
    evaluateDirectionalLight();

    dirAmbient *= texture(diffuseMap, texCoords).rgb;
    dirDiffuse *= texture(diffuseMap, texCoords).rgb;
    dirSpecular *= texture(specularMap, texCoords).rgb;

    resultShadow = evaluateShadowMap();

    vec3 totalColor = min((dirAmbient + (1.0 - resultShadow) * dirDiffuse)
                          + (1.0 - resultShadow) * dirSpecular, 1.0);

    vec3 pointLighting1 = evaluatePointLight(pointPosA, extraLightColorA);
    totalColor += pointLighting1;

    if (fogToggle == 1)
    {
        float fogLevel = evaluateFogFactor();
        vec3 shadeFog = vec3(0.5, 0.5, 0.5);

        vec3 newColorA = vec3(1.0, 1.0, 0.0);
        vec3 pLightA = evaluatePointLight(pointPosB, newColorA);
        vec3 pLightB = evaluatePointLight(pointPosC, newColorA);
        vec3 pLightC = evaluatePointLight(pointPosD, newColorA);
        vec3 pLightD = evaluatePointLight(pointPosE, newColorA);

        totalColor += pLightA + pLightB + pLightC + pLightD;
        outputColor = vec4(mix(shadeFog, totalColor, fogLevel), 1.0);
    }
    else
    {
        outputColor = vec4(totalColor, 1.0);
    }
}
