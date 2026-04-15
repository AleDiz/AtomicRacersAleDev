#include "raylib.h"
#include "bullet/btBulletDynamicsCommon.h"

class RaylibDebugDrawer : public btIDebugDraw {
public:
    int debugMode {};

    RaylibDebugDrawer() : debugMode(btIDebugDraw::DBG_DrawWireframe) {}

    void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override {
        Color rayColor = { (unsigned char)(color.x() * 255), (unsigned char)(color.y() * 255), (unsigned char)(color.z() * 255), 255 };
        DrawLine3D(Vector3{from.x(), from.y(), from.z()}, Vector3{to.x(), to.y(), to.z()}, rayColor);
    }

    void drawContactPoint(const btVector3& pointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) override {}

    void reportErrorWarning(const char* warningString) override {
        TraceLog(LOG_WARNING, "Bullet Warning: %s", warningString);
    }

    void draw3dText(const btVector3& location, const char* textString) override {}

    void setDebugMode(int debugMode) override {
        this->debugMode = debugMode;
    }

    int getDebugMode() const override {
        return debugMode;
    }
};