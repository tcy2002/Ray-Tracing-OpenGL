#include "material.h"

const Material Material::metal = {
        false,
        {0.784f, 0.502f, 0.251f},
        0.8f,
        0.4f,
        0.0f,
        0.0f,
        0.0f,
        1.0f,
        0.0f,
};

const Material Material::plastic = {
        false,
        {0.68f, 0.68f, 0.68f},
        0.0f,
        0.0f,
        0.0f,
        0.0f,
        0.0f,
        1.0f,
        0.0f,
};

const Material Material::glass = {
        false,
        {0.376f, 0.604f, 0.757f},
        0.2f,
        0.8f,
        0.0f,
        0.75f,
        0.8f,
        1.5f,
        0.0f,
};

const Material Material::smoothChina = {
        false,
        {0.9f, 0.9f, 0.9f},
        0.2f,
        0.4f,
        0.3f,
        0.0f,
        0.0f,
        1.0f,
        0.0f,
};

const Material Material::smoothWood = {
        false,
        {0.78f, 0.46f, 0.16f},
        0.4f,
        0.8f,
        0.0f,
        0.0f,
        0.0f,
        1.0f,
        0.0f,
};

const Material Material::wall = {
        false,
        {1.0f, 1.0f, 1.0f},
        0.0f,
        0.0f,
        0.0f,
        0.0f,
        0.0f,
        1.0f,
        0.0f,
};