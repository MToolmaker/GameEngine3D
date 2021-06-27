#include "olcPixelGameEngine.h"

struct vec3 {
    float x, y, z;
};

struct triangle {
    vec3 p1, p2, p3;
};

struct mat4x4 {
    float elements[4 * 4]{};
};

struct mesh {
    std::vector<triangle> triangles;
};


class olcEngine3D : public olc::PixelGameEngine {
public:
    olcEngine3D() {
        sAppName = "GameEngine3D";
    }


private:
    // TODO: Check if anything can be moved to local variables
    constexpr static float myFieldOfView = 135.0f * 3.14f / 180.0f; // FOV in radians
    constexpr static float myFarthestDepth = 1000.0f;
    constexpr static float myCameraDepth = 1.0f;
    float myAspectRation = 1.0f;
    std::vector<mesh> myMeshes;
    mat4x4 myProjectionMatrix;
    float myScalingFactor{};

    static void TransformPoint(const vec3 &input, vec3 &output, const mat4x4& transformation) {
        vec3 tmp = input;
        output.x = tmp.x * transformation.elements[0 * 4 + 0] + tmp.y * transformation.elements[1 * 4 + 0] +
                tmp.z * transformation.elements[2 * 4 + 0] + 1 * transformation.elements[3 * 4 + 0];
        output.y = tmp.x * transformation.elements[0 * 4 + 1] + tmp.y * transformation.elements[1 * 4 + 1] +
                tmp.z * transformation.elements[2 * 4 + 1] + 1 * transformation.elements[3 * 4 + 1];
        output.z = tmp.x * transformation.elements[0 * 4 + 2] + tmp.y * transformation.elements[1 * 4 + 2] +
                tmp.z * transformation.elements[2 * 4 + 2] + 1 * transformation.elements[3 * 4 + 2];
        float w = tmp.x * transformation.elements[0 * 4 + 3] + tmp.y * transformation.elements[1 * 4 + 3] +
                tmp.z * transformation.elements[2 * 4 + 3] + 1 * transformation.elements[3 * 4 + 3];
        if (w == 0.0) return;
        output.x /= w;
        output.y /= w;
        output.z /= w;
    }

public:
    bool OnUserCreate() override {
        // TODO: For future, check if orientation is right wise clock
        mesh myBox;
        myBox.triangles = {

                // SOUTH
                {0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f},
                {0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f},

                // EAST                                                      
                {1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f},
                {1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f},

                // NORTH                                                     
                {1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f},
                {1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f},

                // WEST                                                      
                {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
                {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f},

                // TOP                                                       
                {0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f},
                {0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f},

                // BOTTOM                                                    
                {1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f},
                {1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f},

        };
        myMeshes.push_back(myBox);
        
        
        
        myAspectRation = (float) ScreenHeight() / (float) ScreenWidth();
        myScalingFactor = cos(myFieldOfView / 2.0f) / sin(myFieldOfView / 2.0f);
        myProjectionMatrix.elements[0 * 4 + 0] = myAspectRation * myScalingFactor;
        myProjectionMatrix.elements[1 * 4 + 1] = myScalingFactor;
        float viewableDepthDistance = myFarthestDepth - myCameraDepth;
        myProjectionMatrix.elements[2 * 4 + 2] = myFarthestDepth / viewableDepthDistance;
        myProjectionMatrix.elements[3 * 4 + 2] = -1.0f * myFarthestDepth * myCameraDepth / viewableDepthDistance;
        myProjectionMatrix.elements[2 * 4 + 3] = 1.0f;
        return true;
    }


    float angle{};
    
    bool OnUserUpdate(float fElapsedTime) override {
        FillRect(0, 0, ScreenWidth(), ScreenHeight(), olc::BLACK);
        // TODO: Extract common calculations with cos and sin
        mat4x4 rotationMatrixZ;
        angle += 1.0f * fElapsedTime;
        float cosOfAngle = cos(angle);
        rotationMatrixZ.elements[0 * 4 + 0] = cosOfAngle;
        float sinOfAngle = sin(angle);
        rotationMatrixZ.elements[0 * 4 + 1] = -sinOfAngle;
        rotationMatrixZ.elements[1 * 4 + 0] = sinOfAngle;
        rotationMatrixZ.elements[1 * 4 + 1] = cosOfAngle;
        rotationMatrixZ.elements[2 * 4 + 2] = 1.0f;
        rotationMatrixZ.elements[3 * 4 + 3] = 1.0f;
        
        mat4x4 rotationMatrixX;
        rotationMatrixX.elements[0 * 4 + 0] = 1.0f;
        rotationMatrixX.elements[1 * 4 + 1] = cosOfAngle;
        rotationMatrixX.elements[1 * 4 + 2] = -sinOfAngle;
        rotationMatrixX.elements[2 * 4 + 1] = sinOfAngle;
        rotationMatrixX.elements[2 * 4 + 2] = cosOfAngle;
        rotationMatrixX.elements[3 * 4 + 3] = 1.0f;
        
        vec3 projected{};
        for (const auto &mesh : myMeshes) {
            for (const auto &tri : mesh.triangles) {
                vec3 tmp = tri.p1;
                TransformPoint(tmp, tmp, rotationMatrixX);
                TransformPoint(tmp, tmp, rotationMatrixZ);
                tmp.z += 3.0f;
                TransformPoint(tmp, projected, myProjectionMatrix);
                int p1x = (int)((projected.x + 1.0f) * 0.5f * (float)ScreenWidth());
                int p1y = (int)((projected.y + 1.0f) * 0.5f * (float)ScreenHeight());

                tmp = tri.p2;
                TransformPoint(tmp, tmp, rotationMatrixX);
                TransformPoint(tmp, tmp, rotationMatrixZ);
                tmp.z += 3.0f;
                TransformPoint(tmp, projected, myProjectionMatrix);
                int p2x = (int)((projected.x + 1.0f) * 0.5f * (float)ScreenWidth());
                int p2y = (int)((projected.y + 1.0f) * 0.5f * (float)ScreenHeight());

                tmp = tri.p3;
                TransformPoint(tmp, tmp, rotationMatrixX);
                TransformPoint(tmp, tmp, rotationMatrixZ);
                tmp.z += 3.0f;
                TransformPoint(tmp, projected, myProjectionMatrix);
                int p3x = (int)((projected.x + 1.0f) * 0.5f * (float)ScreenWidth());
                int p3y = (int)((projected.y + 1.0f) * 0.5f * (float)ScreenHeight());

                DrawTriangle(p1x, p1y, p2x, p2y, p3x, p3y, olc::WHITE);
            }

        }
        return true;
    }

};


int main() {
    olcEngine3D demo;
    if (demo.Construct(1920, 1079, 1, 1))
        demo.Start();
    return 0;
}
