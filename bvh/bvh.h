#pragma once

#include <vector>

#include "config/config.h"

struct BVHNode {
    BVHNode *left = nullptr;
    BVHNode *right = nullptr;
    int size = 1;
    int n = 0;
    int index = 0;
    Vector3f AA = {10.0f, 10.0f, 10.0f};
    Vector3f BB = {-10.0f, -10.0f, -10.0f};
};

struct LinearNode {
    Vector3f AA{};
    Vector3f BB{};
    Vector3f lr{};
    Vector3f ni{};
};

class BVH {
private:
    BVHNode *bvh;

    static inline Vector3f min(Vector3f *points, int num);
    static inline Vector3f max(Vector3f *points, int num);

    static inline bool cmpX(const Patch &a, const Patch &b);
    static inline bool cmpY(const Patch &a, const Patch &b);
    static inline bool cmpZ(const Patch &a, const Patch &b);

    BVHNode *buildBVH(std::vector<Patch> &patches, int l, int r, int n);
    void linearize(LinearNode *list, int p, BVHNode *node);

public:
    BVH(std::vector<Patch> &patches, GLsizei num, int max_node);
    GLuint *getLinearBVH(GLsizei &size);
};