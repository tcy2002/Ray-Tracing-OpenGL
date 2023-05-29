#version 450 core

#define PI 3.1415926
#define INF 114514.0
#define ERR 0.0001

in vec3 position;
layout (location = 0) out vec3 FragData;

//屏幕参数
uniform int width;
uniform int height;

//帧数
uniform int frame;
uniform int maxFrame;

//上一帧的帧缓存
uniform sampler2D lastFrame;

//视点
uniform vec3 eyePos;

//表面材质：参考material.h
struct Material {
    bool lighting;
    vec3 color;
    float specularRate;
    float specularTint;
    float specularRoughness;
    float refractRate;
    float refractTint;
    float refractIndex;
    float refractRoughness;
};

//`BVH`树节点
struct BVHNode {
    vec3 AA;
    vec3 BB;
    int l;
    int r;
    int n;
    int index;
};

/*****************************************************
 * 模型定义
 *****************************************************/

//四边形
struct Quad {
    vec3 samples[4];
    vec3 normal;
};

//四边形模型
struct QuadModel {
    Quad quad;
    Material material;
    bool useTexture;
    sampler2D texture;
};

//球体
struct Sphere {
    vec3 center;
    float radius;
};

//球体模型
struct SphereModel {
    Sphere sph;
    Material material;
    bool useTexture;
    sampler2D texture;
};

//圆柱体
struct Cylinder {
    vec3 center;
    float radius;
    float height;
};

//圆柱体模型
struct CylinderModel {
    Cylinder cyl;
    Material material;
    bool useTexture;
    sampler2D texture;
};

//自定义模型
struct CustomizedModel {
    samplerBuffer patchTex;
    samplerBuffer bvhTex;
    vec3 center;
    float height;
    Material material;
    bool useTexture;
    sampler2D texture;
};

/*****************************************************/

//光线
struct Ray {
    vec3 startPoint;
    vec3 direction;
};

//击中信息
struct HitInfo {
    float distance;         // 与交点的距离
    vec3 hitPoint;          // 光线命中点
    vec3 normal;            // 命中点法线
    vec3 viewDir;           // 击中该点的光线的方向
    Material material;      // 命中点的表面材质
};

//模型信息
uniform int quadNum;
uniform QuadModel quads[8];        //最多八个四边形
uniform int sphereNum;
uniform SphereModel spheres[3];    //最多三个球
uniform int cylinderNum;
uniform CylinderModel cylinders[3];//最多三个圆柱体
uniform CustomizedModel customized;//仅支持一个自定义模型

/*****************************************************
 * 生成随机数：随机种子+哈希
 *****************************************************/

//随机种子
uint seed = uint(
    uint((position.x * 0.5 + 0.5) * width) * 1973u +
    uint((position.y * 0.5 + 0.5) * height) * 9277u +
    uint(frame * maxFrame) * 26699u);

//哈希函数
uint hash(inout uint seed) {
    seed *= 0x27d4eb2du;
    seed = seed ^ (seed >> 15);
    return seed;
}

//随机数
float rand() {
    return float(hash(seed)) / 4294967296.0;
}

/*****************************************************
 * sobol序列
 *****************************************************/

uniform uint V[64];

//仅与像素坐标有关的随机种子
uint pseed = uint(
    uint((position.x * 0.5 + 0.5) * width) * 1973u +
    uint((position.y * 0.5 + 0.5) * height) * 9277u +
    512u * 26699u);

//格林码
int gray = frame ^ (frame >> 1);

//生成`sobol`数
float sobol(int d, int i) {
    uint result = 0u;
    int offset = d * 32;
    for (int j = 0, k = i; k != 0; k >>= 1, j++) {
        if ((k & 1) == 1) {
            result ^= V[j + offset];
        }
    }
    return float(result) / 4294967296.0;
}

float CranleyPattersonRotation(float p) {
    float u = float(hash(pseed)) / 4294967296.0;
    p += u;
    if(p > 1.0) p -= 1.0;
    if(p < 0.0) p += 1.0;
    return p;
}

/*****************************************************
 * 生成随机向量
 *****************************************************/

//将向量v投影到N的法向半球
vec3 toNormalHemisphere(vec3 v, vec3 N) {
    vec3 helper = vec3(1.0, 0.0, 0.0);
    if(abs(N.x) >= 1.0 - ERR) helper = vec3(0.0, 0.0, 1.0);
    vec3 tangent = normalize(cross(N, helper));
    vec3 bitangent = normalize(cross(N, tangent));
    return v.x * tangent + v.y * bitangent + v.z * N;
}

//法向半球随机采样
vec3 sampleHemisphere(vec3 N) {
    float r = sqrt(rand());
    float t = rand() * (2.0 * PI);
    float x = r * cos(t);
    float y = r * sin(t);
    float z = sqrt(1.0 - x * x - y * y);
    return toNormalHemisphere(vec3(x, y, z), N);
}

//根据sobol序列的均匀半球采样
vec3 sampleSobolHemisphere(vec3 N) {
    float u = CranleyPattersonRotation(sobol(0, gray));
    float v = CranleyPattersonRotation(sobol(1, gray));
//    float u = sobol(0, gray);
//    float v = sobol(1, gray);
    float r = sqrt(u);
    float t = v * (2.0 * PI);
    float x = r * cos(t);
    float y = r * sin(t);
    float z = sqrt(1.0 - x * x - y * y);
    return toNormalHemisphere(vec3(x, y, z), N);
}

/*****************************************************
 * 光线追踪
 *****************************************************/

//点坐标到四边形纹理坐标的映射
vec2 quadTexCoord(in vec3 samples[4], vec3 P) {
    vec3 m = samples[2] - samples[0];
    vec3 n = samples[0] - samples[1];
    vec3 q = P - samples[1];
    if (m.x == 0.0 && n.x == 0.0 && q.x == 0) {
        mat2 mn = mat2(m.yz, n.yz);
        return inverse(mn) * q.yz;
    }
    if (m.y == 0.0 && n.y == 0.0 && q.y == 0.0) {
        mat2 mn = mat2(m.xz, n.xz);
        return inverse(mn) * q.xz;
    }
    mat2 mn = mat2(m.xy, n.xy);
    return inverse(mn) * n.xy;
}

//光线是否击中四边形
bool hitQuad(Ray r, in Quad quad, inout HitInfo hit) {
    //求光线与平面交点
    vec3 n1 = quad.samples[1] - quad.samples[0];
    vec3 n2 = quad.samples[2] - quad.samples[0];
    vec3 normal = normalize(cross(n1, n2));
    float d = -dot(quad.samples[0], normal);
    float m = dot(r.direction, normal);
    if (m >= -ERR) return false; //剔除背向面
    float t = -(d + dot(r.startPoint, normal)) / m;
    if (t <= ERR) return false; //剔除与自身相交的情况
    vec3 P = r.startPoint + r.direction * t;

    //根据叉乘与法矢量的方向关系判断是否在四边形内
    vec3 n3 = P - quad.samples[0];
    vec3 n4 = P - quad.samples[1];
    vec3 n5 = P - quad.samples[2];
    float f1 = dot(cross(n1, n3), normal);
    float f2 = dot(cross(n3, n2), normal);
    float f3 = dot(cross(n5, n1), normal);
    float f4 = dot(cross(n2, n4), normal);

    if (f1 > -ERR && f2 > -ERR && f3 > -ERR && f4 > -ERR && t < hit.distance - ERR) {
        hit.distance = t;
        hit.hitPoint = P;
        hit.viewDir = r.direction;
        hit.normal = normal;
        return true;
    }

    return false;
}

//光线是否击中四边形模型
bool hitQuadModel(Ray r, in QuadModel quadM, inout HitInfo hit) {
    bool ret = hitQuad(r, quadM.quad, hit);
    if (ret) {
        hit.material = quadM.material;
        //纹理映射
        if (quadM.useTexture) {
            vec2 tex = quadTexCoord(quadM.quad.samples, hit.hitPoint);
            vec3 color = texture(quadM.texture, tex).xyz;
            hit.material.color = color;
        }
    }
    return ret;
}

//光线是否击中球体
bool hitSphere(Ray r, in Sphere sphere, inout HitInfo hit) {
    //计算光线与球心距离
    float t = dot(sphere.center - r.startPoint, r.direction);
    vec3 T = r.startPoint + r.direction * t;
    vec3 CP = T - sphere.center;
    float l_CP = length(CP);

    //距离大于半径则不相交
    if (l_CP > sphere.radius) return false;

    //计算交点
    float delta = sqrt(sphere.radius * sphere.radius - l_CP * l_CP);
    float t1 = t - delta;
    float t2 = t + delta;

    //判断是哪个交点，并剔除与自身相交的情况
    if (t1 > ERR) t = t1;
    else if (t2 > ERR) t = t2;
    else return false;

    //存在遮挡
    if (t >= hit.distance - ERR) return false;

    hit.distance = t;
    hit.hitPoint = r.startPoint + r.direction * t;
    hit.normal = normalize(hit.hitPoint - sphere.center);
    hit.viewDir = r.direction;
    return true;
}

//法矢量到球面纹理坐标的映射
vec2 sphereTexCoord(vec3 N) {
    float ang_x = atan(N.z, N.x);
    float ang_y = asin(N.y);
    vec2 uv = vec2(ang_x, ang_y);
    uv.x = 1.0 - ang_x / (2.0 * PI);
    uv.y = 0.5 + ang_y / PI;
    return uv;
}

//光线是否击中球体模型
bool hitSphereModel(Ray r, in SphereModel sphM, inout HitInfo hit) {
    bool ret = hitSphere(r, sphM.sph, hit);
    if (ret) {
        hit.material = sphM.material;
        //纹理映射
        if (sphM.useTexture) {
            vec2 texc = sphereTexCoord(hit.normal);
            vec3 color = texture(sphM.texture, texc).xyz;
            hit.material.color = color;
        }
        //折射率：射出时需要取倒数
        float ref_ang = hit.material.refractIndex;
        if (ref_ang != 0 && dot(hit.normal, r.direction) > 0) {
            hit.material.refractIndex = 1.0 / ref_ang;
            hit.normal = -hit.normal;
        }
    }
    return ret;
}

//光线是否击中圆柱体
bool hitCylinder(Ray r, in Cylinder cyl, inout HitInfo hit) {
    //计算光线到中轴的最短距离
    vec2 SF = cyl.center.xz - r.startPoint.xz;
    vec2 d_ST = r.direction.xz;
    float l_FT = abs(SF.y * d_ST.x - SF.x * d_ST.y) / length(d_ST);

    //距离大于半径则不与无限长圆柱面相交
    if (l_FT > cyl.radius) return false;

    //计算与无限长圆柱面的交点
    float l_SF = length(SF);
    float t = sqrt(l_SF * l_SF - l_FT * l_FT) / length(d_ST);
    float right = cyl.radius * cyl.radius - l_FT * l_FT;
    float left = 1.0 - r.direction.y * r.direction.y;
    float delta = sqrt(right / left);
    float t1 = t - delta;
    float t2 = t + delta;
    vec3 M = r.startPoint + r.direction * t1;
    vec3 N = r.startPoint + r.direction * t2;

    //交点方向相反
    if (t2 <= ERR) return false;

    //击中点在M
    if (M.y >= cyl.center.y && M.y <= cyl.center.y + cyl.height) {
        if (t1 <= ERR) return false; //与自身相交
        if (t1 >= hit.distance - ERR) return false; //存在遮挡
        vec2 nor = normalize(M.xz - cyl.center.xz);
        hit.distance = t1;
        hit.hitPoint = M;
        hit.normal = vec3(nor.x, 0.0, nor.y);
        hit.viewDir = r.direction;
        return true;
    }

    //击中点在下底面
    if (M.y < cyl.center.y && N.y >= cyl.center.y) {
        float m = (cyl.center.y - r.startPoint.y) / r.direction.y;
        if (m >= hit.distance - ERR) return false; //存在遮挡
        hit.distance = m;
        hit.hitPoint = r.startPoint + r.direction * m;
        hit.normal = vec3(0.0, -1.0, 0.0);
        hit.viewDir = r.direction;
        return true;
    }

    //击中点在上底面
    if (M.y > cyl.center.y + cyl.height && N.y <= cyl.center.y + cyl.height) {
        float m = (cyl.center.y + cyl.height - r.startPoint.y) / r.direction.y;
        if (m >= hit.distance - ERR) return false; //存在遮挡
        hit.distance = m;
        hit.hitPoint = r.startPoint + r.direction * m;
        hit.normal = vec3(0.0, 1.0, 0.0);
        hit.viewDir = r.direction;
        return true;
    }

    return false;
}

//点坐标到圆柱体侧面的纹理映射
vec2 cylinderTexCoord(vec3 P, vec3 center, float height) {
    float ang_x = atan(P.z - center.z, P.x - center.x);
    vec2 uv;
    uv.x = 1.0 - ang_x / (2.0 * PI);
    uv.y = (P.y - center.y) / height;
    return uv;
}

//光线是否击中圆柱体模型
bool hitCylinderModel(Ray r, in CylinderModel cylM, inout HitInfo hit) {
    bool ret = hitCylinder(r, cylM.cyl, hit);
    if (ret) {
        hit.material = cylM.material;
        hit.material.refractRate = 0.0; //圆柱体不支持透明材质
        float y = hit.hitPoint.y;
        float y_l = cylM.cyl.center.y;
        float y_h = y_l + cylM.cyl.height;
        //只有侧面有纹理映射
        if (cylM.useTexture && y > y_l && y < y_h) {
            vec2 tex = cylinderTexCoord(hit.hitPoint, cylM.cyl.center, cylM.cyl.height);
            vec3 color = texture(cylM.texture, tex).xyz;
            hit.material.color = color;
        }
    }
    return ret;
}

//获取自定义模型面片数据
Quad getPatch(int i) {
    int offset = i * 5;
    Quad q;

    q.samples[0] = texelFetch(customized.patchTex, offset).xyz;
    q.samples[1] = texelFetch(customized.patchTex, offset + 1).xyz;
    q.samples[2] = texelFetch(customized.patchTex, offset + 2).xyz;
    q.samples[3] = texelFetch(customized.patchTex, offset + 3).xyz;
    q.normal = texelFetch(customized.patchTex, offset + 4).xyz;

    return q;
}

//获取自定义模型`BVH`树节点数据
BVHNode getBVH(int i) {
    int offset = i * 4;
    BVHNode n;

    n.AA = texelFetch(customized.bvhTex, offset).xyz;
    n.BB = texelFetch(customized.bvhTex, offset + 1).xyz;
    ivec3 tmp = ivec3(texelFetch(customized.bvhTex, offset + 2).xyz);
    n.l = tmp.x;
    n.r = tmp.y;
    tmp = ivec3(texelFetch(customized.bvhTex, offset + 3).xyz);
    n.n = tmp.x;
    n.index = tmp.y;

    return n;
}

//光线是否击中`AABB`包围盒
float hitAABB(Ray r, vec3 AA, vec3 BB) {
    vec3 M = (BB - r.startPoint) / r.direction;
    vec3 N = (AA - r.startPoint) / r.direction;

    vec3 tmax = max(M, N);
    vec3 tmin = min(M, N);

    float t1 = min(tmax.x, min(tmax.y, tmax.z));
    float t2 = max(tmin.x, max(tmin.y, tmin.z));

    return t1 >= t2 && t2 > ERR ? t2 : -1.0;
}

//光线是否击中自定义模型
bool hitCustomizedModel(Ray r, inout HitInfo hit) {
    int stack[8];
    int p = 0;

    stack[p++] = 0;
    while (p > 0) {
        int top = stack[--p];
        BVHNode node = getBVH(top);

        //叶子结点
        if (node.n > 0) {
            int m = node.index;
            int n = m + node.n;
            for (int i = m; i < n; i++) {
                Quad q = getPatch(i);
                if (hitQuad(r, q, hit)) {
                    hit.material = customized.material;
                    hit.material.refractRate = 0.0; //自定义模型不支持透明材质
                    //纹理映射
                    if (customized.useTexture) {
                        vec2 tex = cylinderTexCoord(hit.hitPoint, customized.center, customized.height);
                        vec3 color = texture(customized.texture, tex).xyz;
                        hit.material.color = color;
                    }
                    return true;
                }
            }
        }

        //与左右盒子求交
        float t1 = -1.0, t2 = -1.0;
        if (node.l >= 0) {
            BVHNode l_node = getBVH(node.l);
            t1 = hitAABB(r, l_node.AA, l_node.BB);
        }
        if (node.r >= 0) {
            BVHNode r_node = getBVH(node.r);
            t2 = hitAABB(r, r_node.AA, r_node.BB);
        }

        //在最近的盒子中搜索
        if (t1 > 0 && t2 > 0) {
            if (t1 < t2) {
                stack[p++] = node.r;
                stack[p++] = node.l;
            } else {
                stack[p++] = node.l;
                stack[p++] = node.r;
            }
        } else if (t1 > 0) {
            stack[p++] = node.l;
        } else if (t2 > 0) {
            stack[p++] = node.r;
        }
    }

    return false;
}

//击中判断
bool hitModel(Ray r, out HitInfo hit) {
    hit.distance = INF;
    bool ret = false;

    for (int i = 0; i < cylinderNum; i++) {
        ret = hitCylinderModel(r, cylinders[i], hit) || ret;
    }
    for (int i = 0; i < quadNum; i++) {
        ret = hitQuadModel(r, quads[i], hit) || ret;
    }
    for (int i = 0; i < sphereNum; i++) {
        ret = hitSphereModel(r, spheres[i], hit) || ret;
    }
    ret = hitCustomizedModel(r, hit) || ret;

    return ret;
}

//路径追踪：线性化递归
vec3 pathTracing(Ray r, int maxDepth) {
    if (maxDepth > 8) maxDepth = 8; //最多递归八层
    vec3 color[8];   //记录每一层递归的基础颜色
    int type[8];     //记录每一层递归的光线类型
    float cosine[8]; //记录每一层递归的夹角余弦
    float tint[8];   //记录每一层递归的混合指数
    int depth;

    for (depth = 0; depth < maxDepth; depth++) {
        //若未击中则直接返回
        HitInfo hit;
        if (!hitModel(r, hit)) {
            color[depth] = vec3(0.0);
            break;
        }

        //反伽马校正
        color[depth] = pow(hit.material.color, vec3(2.2));
//        color[depth] = hit.material.color;

        //若击中光源则返回
        if (hit.material.lighting) {
            color[depth] *= 2;
            break;
        }

        //光线与击中点法矢量的夹角余弦
        cosine[depth] = abs(dot(hit.normal, r.direction));

        //随机生成下一条光线
        vec3 oldRay = r.direction;
        r.direction = depth == 0 ? sampleSobolHemisphere(hit.normal) : sampleHemisphere(hit.normal);
//        r.direction = sampleHemisphere(hit.normal);
        r.startPoint = hit.hitPoint;

        //根据物体材质决定下一条光线的方向
        float p = rand();
        //镜面反射
        if (p < hit.material.specularRate) {
            //镜面反射
            vec3 ref = reflect(oldRay, hit.normal);
            r.direction = normalize(mix(ref, r.direction, hit.material.specularRoughness));
            tint[depth] = hit.material.specularTint;
            type[depth] = 1;
        } else if (hit.material.specularRate <= p && p <= hit.material.specularRate + hit.material.refractRate) {
            //折射
            vec3 ref = refract(oldRay, hit.normal, 1.0 / hit.material.refractIndex);
            r.direction = normalize(mix(ref, -r.direction, hit.material.refractRoughness));
            tint[depth] = hit.material.refractTint;
            type[depth] = 2;
        } else {
            //漫反射
            type[depth] = 0;
        }
    }

    //计算累积颜色
    for (int i = depth - 1; i >= 0; i--) {
        vec3 light = color[i + 1] * sqrt(cosine[i]);
        if (type[i] > 0) {
            color[i] = mix(color[i] * length(light), light, tint[i]);
        } else {
            color[i] *= light;
        }
    }

    return color[0];
}

void main() {
    //前一帧
    vec2 pixel = position.xy * 0.5 + 0.5;
    vec3 lastColor = texture(lastFrame, pixel).xyz;
    if (frame >= maxFrame) {
        FragData = lastColor;
        return;
    }

    //初始光线方向为视点指向像素点，加入随机偏移量以抗锯齿
    Ray r;
    r.startPoint = eyePos;
    vec3 screen = position;
    float d = rand(), th = rand() * (2.0 * PI);
    screen.x += (d * sin(th) - 0.5) * (2.0 / width);
    screen.y += (d * cos(th) - 0.5) * (2.0 / height);
    r.direction = normalize(screen - eyePos);

    //当前帧的像素颜色加上前一帧的像素颜色
    vec3 color = pathTracing(r, 6);
    float rate = 1.0 / (frame + 1);
//    FragData = mix(lastColor, color * (2.0 * PI), rate);
    FragData = lastColor + color * (2.0 * PI);
}