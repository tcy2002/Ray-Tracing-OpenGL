#include <algorithm>

#include "bvh.h"

BVH::BVH(std::vector<Patch> &patches, GLsizei num, int max_node) {
    bvh = buildBVH(patches, 0, num - 1, max_node);
}

Vector3f BVH::min(Vector3f *points, int num) {
    float min_x = 10.0f, min_y = 10.0f, min_z = 10.0f;
    for (int i = 0; i < num; i++) {
        if (points[i].x < min_x) min_x = points[i].x;
        if (points[i].y < min_y) min_y = points[i].y;
        if (points[i].z < min_z) min_z = points[i].z;
    }
    return {min_x, min_y, min_z};
}

Vector3f BVH::max(Vector3f *points, int num) {
    float max_x = -10.0f, max_y = -10.0f, max_z = -10.0f;
    for (int i = 0; i < num; i++) {
        if (points[i].x > max_x) max_x = points[i].x;
        if (points[i].y > max_y) max_y = points[i].y;
        if (points[i].z > max_z) max_z = points[i].z;
    }
    return {max_x, max_y, max_z};
}

bool BVH::cmpX(const Patch &a, const Patch &b) {
    return a.samples[0].x + a.samples[1].x + a.samples[2].x + a.samples[3].x
            < b.samples[0].x + b.samples[1].x + b.samples[2].x + b.samples[3].x;
}

bool BVH::cmpY(const Patch &a, const Patch &b) {
    return a.samples[0].y + a.samples[1].y + a.samples[2].y + a.samples[3].y
           < b.samples[0].y + b.samples[1].y + b.samples[2].y + b.samples[3].y;
}

bool BVH::cmpZ(const Patch &a, const Patch &b) {
    return a.samples[0].z + a.samples[1].z + a.samples[2].z + a.samples[3].z
           < b.samples[0].z + b.samples[1].z + b.samples[2].z + b.samples[3].z;
}

BVHNode *BVH::buildBVH(std::vector<Patch> &patches, int l, int r, int n) {
    if (l > r) return nullptr;
    auto *node = new BVHNode();

    Vector3f tmp;
    for (int i = l; i <= r; i++) {
        //最小点AA
        tmp = min(patches[i].samples, 4);
        if (tmp.x < node->AA.x) node->AA.x = tmp.x;
        if (tmp.y < node->AA.y) node->AA.y = tmp.y;
        if (tmp.z < node->AA.z) node->AA.z = tmp.z;
        //最大点BB
        tmp = max(patches[i].samples, 4);
        if (tmp.x > node->BB.x) node->BB.x = tmp.x;
        if (tmp.y > node->BB.y) node->BB.y = tmp.y;
        if (tmp.z > node->BB.z) node->BB.z = tmp.z;
    }

    //不多于n个面片时递归结束
    if (r - l + 1 <= n) {
        node->n = r - l + 1;
        node->index = l;
        return node;
    }

    //否则递归建树
    float len_x = node->BB.x - node->AA.x;
    float len_y = node->BB.y - node->AA.y;
    float len_z = node->BB.z - node->AA.z;

    if (len_x >= len_y && len_x >= len_z) //按x划分
        std::sort(patches.begin() + l, patches.begin() + r + 1, cmpX);
    else if (len_y >= len_z) //按y划分
        std::sort(patches.begin() + l, patches.begin() + r + 1, cmpY);
    else //按z划分
        std::sort(patches.begin() + l, patches.begin() + r + 1, cmpZ);

    int mid = (l + r) / 2;
    node->left = buildBVH(patches, l, mid, n);
    node->right = buildBVH(patches, mid + 1, r, n);
    if (node->left != nullptr) node->size += node->left->size;
    if (node->right != nullptr) node->size += node->right->size;
    return node;
}

void BVH::linearize(LinearNode *list, int p, BVHNode *node) {
    list[p].ni = {GLfloat(node->n), GLfloat(node->index), 0};
    list[p].AA = node->AA;
    list[p].BB = node->BB;

    int n = p + 1, l = -1, r = -1;
    if (node->left != nullptr) {
        l = n;
        linearize(list, n, node->left);
        n += node->left->size;
    }
    if (node->right != nullptr) {
        r = n;
        linearize(list, n, node->right);
    }

    list[p].lr = {GLfloat(l), GLfloat(r), 0};
}

GLuint *BVH::getLinearBVH(GLsizei &size) {
    if (bvh == nullptr) {
        size = 0;
        return nullptr;
    }
    auto buf = new LinearNode[bvh->size];
    linearize(buf, 0, bvh);

//    std::cout << bvh->size << std::endl;
//    for (int i = 0; i < bvh->size; i++) {
//        std::cout << i <<
//        " n index: " << buf[i].ni <<
//        " l r: " << buf[i].lr <<
//        " AA: " << buf[i].AA <<
//        " BB: " << buf[i].BB << std::endl;
//    }

//    exit(0);

    size = (GLsizei)(bvh->size * sizeof(LinearNode));
    return (GLuint *)buf;
}

