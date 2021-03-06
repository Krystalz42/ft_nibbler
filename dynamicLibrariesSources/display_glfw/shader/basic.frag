#version 330 core
out vec4 FragColor;

struct tMaterial
{
    int     isActive;
    float	shininess;
	vec3	ambient;
	vec3	diffuse;
	vec3	specular;
};

struct tLight
{
	vec3	position;
	float	intensity;
};

smooth in vec2 TexCoords;
smooth in vec3 Normal;
smooth in vec3 Position;

uniform sampler2D texture_diffuse1;
uniform int uBackground;
uniform vec3 uCameraPosition;
uniform tLight     		uLight;
uniform tMaterial		uMaterial;
tMaterial material;

const int levels = 4;
const float scaleFactor = 1.f / levels;

float celShading(float intensity)
{
    if (intensity > 0.93f)
        return (1.f);
    else if (intensity > 0.6f)
        return (0.8f);
    else if (intensity > 0.4f)
        return (0.6f);
    else if (intensity > 0.2f)
        return (0.4f);
    else if (intensity > 0.1f)
        return (0.1f);
    return (0.f);
}

vec3	phong()
{
    vec3 normal = normalize(Normal);
    vec3 posToLight = normalize(uLight.position - Position);
    vec3 posToCamera = normalize(uCameraPosition - Position);
    vec3 cameraToLight = normalize(posToLight + posToCamera);

    float diffuse =  max(dot(normal, posToLight), 0);
    vec3 diffuseColor = uMaterial.diffuse * celShading(diffuse);
    if (uBackground == 1)
        diffuseColor = uMaterial.diffuse;

    vec3 reflectDir = reflect(-posToLight, normal);
    float specular = pow(max(dot(posToCamera, reflectDir), 0.f), uMaterial.shininess);
    float specMask = (pow(dot(cameraToLight, normal), uMaterial.shininess) > 0.4f) ? 1.f : 0.f;

    vec3 color = (uMaterial.ambient + diffuseColor + (uMaterial.specular * specular * specMask)) * uLight.intensity;
    return (color);
}

void main()
{
    if (uMaterial.isActive == 0) {
        FragColor = vec4(0.f, 0.f, 0.f, 1.0f);
        return ;
    }

    material = uMaterial;
    vec4 color;
    if (1 == 1) {
        color = vec4(phong(), 1.f);
    }
    else {
        color = texture(texture_diffuse1, TexCoords);
        material.diffuse = color.rgb;
        material.ambient = color.rgb;
        material.specular = color.rgb * 0.1f;
        color = vec4(phong(), 1.f);
    }
    FragColor = vec4(color.rgb, 1.0f);
}