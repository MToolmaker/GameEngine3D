#include <strstream>
#include "olcPixelGameEngine.h"

struct vec3 {
    float x, y, z;
};

struct triangle {
    vec3 p1, p2, p3;
    olc::Pixel color;
};

struct mat4x4 {
    float elements[4 * 4]{};
};

struct mesh {
    std::vector<triangle> triangles;

    bool LoadFrom(const std::string &sFilename) {
        std::ifstream file(sFilename);
        if (!file.is_open()) return false;

        std::vector<vec3> vertexes;
        while (!file.eof()) {
            char line[128];
            file.getline(line, 128);

            std::strstream s;
            s << line;

            char junk;

            if (line[0] == 'v') {
                vec3 v{};
                s >> junk >> v.x >> v.y >> v.z;
                vertexes.push_back(v);
            }

            if (line[0] == 'f') {
                int trianglesOrder[3];
                s >> junk >> trianglesOrder[0] >> trianglesOrder[1] >> trianglesOrder[2];
                triangles.push_back({vertexes[trianglesOrder[0] - 1], vertexes[trianglesOrder[1] - 1],
                                     vertexes[trianglesOrder[2] - 1]});
            }
        }

        return true;
    }

};

struct screenPosition {
    int w, h;
};

class olcEngine3D : public olc::PixelGameEngine {
public:
    olcEngine3D() {
        sAppName = "GameEngine3D";
    }

    bool OnUserCreate() override {
        // TODO: For future, check if orientation is right wise clock
        mesh cargoShips;
//        cargoShips.triangles = {
//
//                // SOUTH
//                {0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f},
//                {0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f},
//
//                // EAST                                                      
//                {1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f},
//                {1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f},
//
//                // NORTH                                                     
//                {1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f},
//                {1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f},
//
//                // WEST                                                      
//                {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
//                {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f},
//
//                // TOP                                                       
//                {0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f},
//                {0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f},
//
//                // BOTTOM                                                    
//                {1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f},
//                {1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f},
//
//        };
        cargoShips.LoadFrom("cat.obj");
        myMeshes.push_back(cargoShips);


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
        angle += 0.8f * fElapsedTime;
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

        mat4x4 rotationMatrixY;
        rotationMatrixY.elements[0 * 4 + 0] = cosOfAngle;
        rotationMatrixY.elements[0 * 4 + 2] = sinOfAngle;
        rotationMatrixY.elements[1 * 4 + 1] = 1.0f;
        rotationMatrixY.elements[2 * 4 + 0] = -sinOfAngle;
        rotationMatrixY.elements[2 * 4 + 2] = cosOfAngle;
        rotationMatrixY.elements[3 * 4 + 3] = 1.0f;

        vec3 projected{};
        vec3 translated{};
        vec3 rotatedX{};
        vec3 rotatedXZ{};
#pragma clang diagnostic push
#pragma ide diagnostic ignored "UnusedValue"
        vec3 translated1{}, translated2{}, translated3{};
#pragma clang diagnostic pop
        vec3 normal{}, line1{}, line2{};
        for (const auto &mesh : myMeshes) {
            for (const auto &tri : mesh.triangles) {
                RotateAndMovePoint(rotationMatrixZ, rotationMatrixX, tri, translated, rotatedX, rotatedXZ, tri.p1);
                translated1 = translated;
                RotateAndMovePoint(rotationMatrixZ, rotationMatrixX, tri, translated, rotatedX, rotatedXZ, tri.p2);
                translated2 = translated;
                RotateAndMovePoint(rotationMatrixZ, rotationMatrixX, tri, translated, rotatedX, rotatedXZ, tri.p3);
                translated3 = translated;
                line1.x = translated2.x - translated1.x;
                line1.y = translated2.y - translated1.y;
                line1.z = translated2.z - translated1.z;

                line2.x = translated3.x - translated1.x;
                line2.y = translated3.y - translated1.y;
                line2.z = translated3.z - translated1.z;

                normal.x = line1.y * line2.z - line1.z * line2.y;
                normal.y = line1.z * line2.x - line1.x * line2.z;
                normal.z = line1.x * line2.y - line1.y * line2.x;
                float length = sqrtf(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
                normal.x /= length;
                normal.y /= length;
                normal.z /= length;

                if (normal.x * (translated1.x - myCamera.x) +
                    normal.y * (translated1.y - myCamera.y) +
                    normal.z * (translated1.z - myCamera.z) >= 0.0f)
                    continue;
                vec3 lightDirection = {0.0f, 0.0f, -1.0f};
                float alignmentWithLight = normal.x * lightDirection.x +
                                           normal.y * lightDirection.y +
                                           normal.z * lightDirection.z;
                float darkColor = 20.0f;
                float lightMaxIntensityDelta = 170.0f;
                int shadingIntensity = (int) (darkColor + abs(alignmentWithLight) * lightMaxIntensityDelta);
                auto color = olc::Pixel(shadingIntensity, shadingIntensity, shadingIntensity);
                triangle transformedTriangle = {translated1, translated2, translated3, color};
                trianglesToRaster.push_back(transformedTriangle);
            }
        }

        SortTrianglesByAverageZ();

        for (const auto &tri : trianglesToRaster) {
            screenPosition first = ProjectPointOnScreen(tri.p1, projected);
            screenPosition second = ProjectPointOnScreen(tri.p2, projected);
            screenPosition third = ProjectPointOnScreen(tri.p3, projected);
            FillTriangle(first.w, first.h, second.w, second.h, third.w, third.h, tri.color);
        }
        trianglesToRaster.clear();
        return true;
    }

    void SortTrianglesByAverageZ() {
        std::sort(trianglesToRaster.begin(), trianglesToRaster.end(), [](triangle &a, triangle &b) {
            float firstAverageZ = (a.p1.z + a.p2.z + a.p3.z) / 3.0f;
            float secondAverageZ = (b.p1.z + b.p2.z + b.p3.z) / 3.0f;
            return firstAverageZ > secondAverageZ;
        });
    }

    static void RotateAndMovePoint(const mat4x4 &rotationMatrixZ, const mat4x4 &rotationMatrixX, const triangle &tri,
                                   vec3 &translated, vec3 &rotatedX, vec3 &rotatedXZ, const vec3 &input) {
        TransformPoint(input, rotatedX, rotationMatrixX);
        TransformPoint(rotatedX, rotatedXZ, rotationMatrixZ);
        translated = rotatedXZ;
        translated.z += 8.0f;
    }

private:
    // TODO: Check if anything can be moved to local variables
    constexpr static float myFieldOfView = 135.0f * 3.14f / 180.0f; // FOV in radians
    constexpr static float myFarthestDepth = 1000.0f;
    constexpr static float myCameraDepth = 0.00010f;
    float myAspectRation = 1.0f;
    std::vector<mesh> myMeshes;
    mat4x4 myProjectionMatrix;
    float myScalingFactor{};
    vec3 myCamera{};
    std::vector<triangle> trianglesToRaster{};

    static void TransformPoint(const vec3 &input, vec3 &output, const mat4x4 &transformation) {
        output.x = input.x * transformation.elements[0 * 4 + 0] + input.y * transformation.elements[1 * 4 + 0] +
                   input.z * transformation.elements[2 * 4 + 0] + 1 * transformation.elements[3 * 4 + 0];
        output.y = input.x * transformation.elements[0 * 4 + 1] + input.y * transformation.elements[1 * 4 + 1] +
                   input.z * transformation.elements[2 * 4 + 1] + 1 * transformation.elements[3 * 4 + 1];
        output.z = input.x * transformation.elements[0 * 4 + 2] + input.y * transformation.elements[1 * 4 + 2] +
                   input.z * transformation.elements[2 * 4 + 2] + 1 * transformation.elements[3 * 4 + 2];
        float w = input.x * transformation.elements[0 * 4 + 3] + input.y * transformation.elements[1 * 4 + 3] +
                  input.z * transformation.elements[2 * 4 + 3] + 1 * transformation.elements[3 * 4 + 3];
        if (w == 0.0) return;
        output.x /= w;
        output.y /= w;
        output.z /= w;
    }

    screenPosition ProjectPointOnScreen(const vec3 &translated, vec3 &projected) const {
        TransformPoint(translated, projected, myProjectionMatrix);
        float width = ((projected.x + 1.0f) * 0.5f * (float) ScreenWidth());
        float height = ((projected.y + 1.0f) * 0.5f * (float) ScreenHeight());
        return screenPosition{static_cast<int>(width), static_cast<int>(height)};
    }
};


int main() {
    olcEngine3D demo;
    if (demo.Construct(1920, 1080, 1, 1))
        demo.Start();
    return 0;
}
