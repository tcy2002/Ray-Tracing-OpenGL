#include "scene.h"
#include "shader/shaderBuf.h"

Scene::Scene() {
    tracerShader = new Shader(tracer_vert, tracer_frag);
    renderShader = new Shader(tracer_vert, render_frag);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(screen), screen, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3f), nullptr);

    //启用帧缓冲
    glGenFramebuffers(1, &fbo);
    glGenTextures(1, &tbo);
    glBindTexture(GL_TEXTURE_2D, tbo);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, ORI_WIDTH, ORI_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //设置模型
    Material *mat;
    Texture *tex;
    Model *mod;
    //左灯
    mat = new Material();
    mat->lighting = true;
    mat->color = {0.95f, 0.95f, 0.95f};
    mod = new QuadModel({-1.0f, 0.8f, -0.7f},
                        {-1.0f, 0.2f, -0.7f},
                        {-1.0f, 0.8f, -1.3f},
                        mat);
    models.push_back(mod);
    //右灯
    mat = new Material();
    mat->lighting = true;
    mat->color = {0.95f, 0.95f, 0.95f};
    mod = new QuadModel({1.0f, 0.8f, -1.3f},
                        {1.0f, 0.2f, -1.3f},
                        {1.0f, 0.8f, -0.7f},
                        mat);
    models.push_back(mod);
    //后墙
    mat = new Material(Material::wall);
    mat->color = {0.6f, 0.85f, 0.918f};
    mod = new QuadModel({-1.0f, 1.0f, -2.0f},
                        {-1.0f, -1.0f, -2.0f},
                        {1.0f, 1.0f, -2.0f},
                        mat);
    models.push_back(mod);
    //左墙
    mat = new Material(Material::wall);
    mat->color = {1.0f, 0.682f, 0.788f};
    mod = new QuadModel({-1.0f, 1.0f, 0.0f},
                        {-1.0f, -1.0f, 0.0f},
                        {-1.0f, 1.0f, -2.0f},
                        mat);
    models.push_back(mod);
    //右墙
    mat = new Material(Material::wall);
    mat->color = {0.71f, 0.902f, 0.114f};
    mod = new QuadModel({1.0f, 1.0f, -2.0f},
                        {1.0f, -1.0f, -2.0f},
                        {1.0f, 1.0f, 0.0f},
                        mat);
    models.push_back(mod);
    //天花板
    mat = new Material(Material::wall);
    mat->color = {1.0f, 1.0f, 1.0f};
    mod = new QuadModel({-1.0f, 1.0f, 0.0f},
                        {-1.0f, 1.0f, -2.0f},
                        {1.0f, 1.0f, 0.0f},
                        mat);
    models.push_back(mod);
    //地板
    mat = new Material(Material::smoothWood);
    tex = new Texture(".\\static\\3.bmp");
    mod = new QuadModel({-1.0f, -1.0f, -2.0f},
                        {-1.0f, -1.0f, 0.0f},
                        {1.0f, -1.0f, -2.0f},
                        mat, tex);
    models.push_back(mod);
//    //前墙
//    mat = new Material(Material::wall);
//    mod = new QuadModel({1.0f, 1.0f, 0.0f},
//                        {1.0f, -1.0f, 0.0f},
//                        {-1.0f, 1.0f, 0.0f},
//                        mat);
//    models.push_back(mod);
    //玻璃球
    mat = new Material(Material::glass);
    mod = new SphereModel({0.6f, -0.1f, -0.7f},
                          0.25f, mat);
    models.push_back(mod);
    //塑料圆柱1
    mat = new Material(Material::plastic);
    mod = new CylinderModel({0.6f, -1.0f, -0.7f},
                            0.15f, 0.65f, mat);
    models.push_back(mod);
    //金属球
    mat = new Material(Material::metal);
    mod = new SphereModel({0.4f, 0.15f, -1.5f},
                          0.15f, mat);
    models.push_back(mod);
    //塑料圆柱2
    mat = new Material(Material::plastic);
    mod = new CylinderModel({0.4f, -1.0f, -1.5f},
                            0.1f, 1.0f, mat);
    models.push_back(mod);
    //旋转扫描模型
    mat = new Material(Material::smoothChina);
    tex = new Texture(".\\static\\2000.bmp");
    mod = new CustomizedModel(".\\static\\goblet.obj", eyePos, mat, tex);
    mod->trans(0.5f, {-0.6f, -0.2f - mod->getCenter().y * 0.5f, -1.5f});
    mod->build();
    models.push_back(mod);
    //塑料圆柱3
    mat = new Material(Material::plastic);
    mod = new CylinderModel({-0.6f, -1.0f, -1.5f},
                            0.25, 0.8f, mat);
    models.push_back(mod);
    //塑料地球仪
    mat = new Material(Material::plastic);
    tex = new Texture(".\\static\\10.bmp");
    mod = new SphereModel({-0.1f, -0.6f, -1.0f},
                          0.4f, mat, tex);
    models.push_back(mod);

    //记录开始时间
    start = GetTickCount();
}

Scene::~Scene() {
    glDeleteFramebuffers(1, &fbo);
    glDeleteTextures(1, &tbo);
}

void Scene::setMaterial(const std::string &name, Material *material) {
    tracerShader->setBool(name + ".lighting", material->lighting);
    tracerShader->setVec3(name + ".color", material->color);
    tracerShader->setFloat(name + ".specularRate", material->specularRate);
    tracerShader->setFloat(name + ".specularTint", material->specularTint);
    tracerShader->setFloat(name + ".specularRoughness", material->specularRoughness);
    tracerShader->setFloat(name + ".refractRate", material->refractRate);
    tracerShader->setFloat(name + ".refractTint", material->refractTint);
    tracerShader->setFloat(name + ".refractIndex", material->refractIndex);
    tracerShader->setFloat(name + ".refractRoughness", material->refractRoughness);
}

void Scene::setTexture(const std::string &name, Texture *texture, int id) {
    if (texture != nullptr) {
        tracerShader->setBool(name + ".useTexture", true);
        //前18个纹理单元已被占用
        texture->bind(GL_TEXTURE18 + id);
        tracerShader->setInt(name + ".texture", 18 + id);
    } else {
        tracerShader->setBool(name + ".useTexture", false);
    }
}

void Scene::setQuad(const std::string &name, Model *model) {
    tracerShader->setVec3(name + ".quad.samples", model->getSamples(), 4);
    tracerShader->setVec3(name + ".quad.normal", model->getNormal());
    setMaterial(name + ".material", model->getMaterial());
    setTexture(name, model->getTexture(), model->getId());
}

void Scene::setSphere(const std::string &name, Model *model) {
    tracerShader->setVec3(name + ".sph.center", model->getCenter());
    tracerShader->setFloat(name + ".sph.radius", model->getRadius());
    setMaterial(name + ".material", model->getMaterial());
    setTexture(name, model->getTexture(), model->getId());
}

void Scene::setCylinder(const std::string &name, Model *model) {
    tracerShader->setVec3(name + ".cyl.center", model->getCenter());
    tracerShader->setFloat(name + ".cyl.radius", model->getRadius());
    tracerShader->setFloat(name + ".cyl.height", model->getHeight());
    setMaterial(name + ".material", model->getMaterial());
    setTexture(name, model->getTexture(), model->getId());
}

void Scene::setCustomized(const std::string &name, Model *model) {
    glActiveTexture(GL_TEXTURE16);
    glBindTexture(GL_TEXTURE_BUFFER, model->getPatchTex());
    tracerShader->setInt(name + ".patchTex", 16);
    glActiveTexture(GL_TEXTURE17);
    glBindTexture(GL_TEXTURE_BUFFER, model->getBVHTex());
    tracerShader->setInt(name + ".bvhTex", 17);
    tracerShader->setVec3(name + ".center", model->getCenter());
    tracerShader->setFloat(name + ".height", model->getHeight());
    setMaterial(name + ".material", model->getMaterial());
    setTexture(name, model->getTexture(), model->getId());
}

void Scene::hitModel(GLfloat x, GLfloat y) {
    Vector3f screenPoint = {x, y, 0.0f};
    Ray r = {normalize(screenPoint - eyePos), eyePos};
    GLfloat minDist = 10.0f, t;
    int size = (int)models.size(), hit = -1;

    for (int i = 0; i < size; i++) {
        t = models[i]->hit(r);
        if (t > 0 && t < minDist) {
            minDist = t;
            hit = i;
        }
    }

    if (hit != -1) {
        models[hit]->setLighting();
        //重置
        frame = 0;
        finished = false;
        start = GetTickCount();
    }
}

void Scene::render() {
    int nums[4]{};
    char buf[20];

    if (!finished) {
        //绑定自定义帧缓存
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        tracerShader->use();
        tracerShader->setInt("width", ORI_WIDTH);
        tracerShader->setInt("height", ORI_HEIGHT);
        tracerShader->setInt("frame", frame);
        tracerShader->setInt("maxFrame", MAX_FRAME);
        tracerShader->setVec3("eyePos", eyePos);
        tracerShader->setUint("V", (GLuint *) sobol, 64);

        //将前一帧的纹理加载到光线追踪着色器中
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tbo);
        tracerShader->setInt("lastFrame", 0);

        //将模型数据导入光线追踪着色器
        for (auto model: models) {
            switch (model->type()) {
                case QUAD:
                    sprintf(buf, "quads[%d]", nums[QUAD]++);
                    setQuad(buf, model);
                    break;
                case SPHERE:
                    sprintf(buf, "spheres[%d]", nums[SPHERE]++);
                    setSphere(buf, model);
                    break;
                case CYLINDER:
                    sprintf(buf, "cylinders[%d]", nums[CYLINDER]++);
                    setCylinder(buf, model);
                    break;
                case CUSTOMIZED:
                    setCustomized("customized", model);
                    break;
                default:
                    break;
            }
        }

        tracerShader->setInt("quadNum", nums[QUAD]);
        tracerShader->setInt("sphereNum", nums[SPHERE]);
        tracerShader->setInt("cylinderNum", nums[CYLINDER]);

        //将渲染结果加载到纹理中
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, tbo, 0);

        //绘制屏幕像素
        glBindVertexArray(VAO);
        glEnableVertexAttribArray(1);
        glDrawArrays(GL_QUADS, 0, 4);
    }

    //重新绑定到默认帧缓存
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    renderShader->use();
    renderShader->setInt("maxFrame", MAX_FRAME);

    //将当前帧的纹理加载到绘制着色器中
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tbo);
    renderShader->setInt("frameBuffer", 0);

    //再次绘制屏幕像素
    glBindVertexArray(VAO);
    glEnableVertexAttribArray(1);
    glDrawArrays(GL_QUADS, 0, 4);

    //更新帧数
    if (frame < MAX_FRAME) {
        frame++;
    } else if (!finished){
        finished = true;
        double rate = 1000.0 * MAX_FRAME / (GetTickCount() - start);
        std::cout << "fn: " << MAX_FRAME << " fps: " << rate << std::endl;
    }
}
