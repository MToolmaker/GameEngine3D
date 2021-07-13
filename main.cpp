#include <strstream>
#include "olcPixelGameEngine.h"

struct vec3 {
    float x{}, y{}, z{};
    float w = 1;
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

struct screenVector {
    int32_t w, h;
};

struct screenTriangle {
    screenVector p1, p2, p3;
};

class olcEngine3D : public olc::PixelGameEngine {
public:
    olcEngine3D() {
        sAppName = "GameEngine3D";
    }

    bool OnUserCreate() override {
        LoadMeshes();
        SetupTransformationMatrices();
        return true;
    }

    void SetupTransformationMatrices() {
        float aspectRatio = (float) ScreenHeight() / (float) ScreenWidth();
        myProjectionMatrix = CreateProjectionMatrix(FieldOfView, aspectRatio, CameraDepth, FarthestDepth);
    }

    void LoadMeshes() {
        mesh myBox;
        myBox.triangles = {

                // SOUTH
                {0.0f, 0.0f, 0.0f + 5.3f, 1.0f, 0.0f, 1.0f, 0.0f + 5.3f, 1.0f, 1.0f, 1.0f, 0.0f + 5.3f},
                {0.0f, 0.0f, 0.0f + 5.3f, 1.0f, 1.0f, 1.0f, 0.0f + 5.3f, 1.0f, 1.0f, 0.0f, 0.0f + 5.3f},

                // EAST                                                      
                {1.0f, 0.0f, 0.0f + 5.3f, 1.0f, 1.0f, 1.0f, 0.0f + 5.3f, 1.0f, 1.0f, 1.0f, 1.0f + 5.3f},
                {1.0f, 0.0f, 0.0f + 5.3f, 1.0f, 1.0f, 1.0f, 1.0f + 5.3f, 1.0f, 1.0f, 0.0f, 1.0f + 5.3f},

                // NORTH                                                     
                {1.0f, 0.0f, 1.0f + 5.3f, 1.0f, 1.0f, 1.0f, 1.0f + 5.3f, 1.0f, 0.0f, 1.0f, 1.0f + 5.3f},
                {1.0f, 0.0f, 1.0f + 5.3f, 1.0f, 0.0f, 1.0f, 1.0f + 5.3f, 1.0f, 0.0f, 0.0f, 1.0f + 5.3f},

                // WEST                                                      
                {0.0f, 0.0f, 1.0f + 5.3f, 1.0f, 0.0f, 1.0f, 1.0f + 5.3f, 1.0f, 0.0f, 1.0f, 0.0f + 5.3f},
                {0.0f, 0.0f, 1.0f + 5.3f, 1.0f, 0.0f, 1.0f, 0.0f + 5.3f, 1.0f, 0.0f, 0.0f, 0.0f + 5.3f},

                // TOP                                                       
                {0.0f, 1.0f, 0.0f + 5.3f, 1.0f, 0.0f, 1.0f, 1.0f + 5.3f, 1.0f, 1.0f, 1.0f, 1.0f + 5.3f},
                {0.0f, 1.0f, 0.0f + 5.3f, 1.0f, 1.0f, 1.0f, 1.0f + 5.3f, 1.0f, 1.0f, 1.0f, 0.0f + 5.3f},

                // BOTTOM                                                    
                {1.0f, 0.0f, 1.0f + 5.3f, 1.0f, 0.0f, 0.0f, 1.0f + 5.3f, 1.0f, 0.0f, 0.0f, 0.0f + 5.3f},
                {1.0f, 0.0f, 1.0f + 5.3f, 1.0f, 0.0f, 0.0f, 0.0f + 5.3f, 1.0f, 1.0f, 0.0f, 0.0f + 5.3f},

        };
        myMeshes.push_back(myBox);
    }


    static mat4x4 CreateProjectionMatrix(float fieldOfViewInDegrees, float fAspectRatio, float fNear, float fFar) {
        float halfOfFieldOfViewInRadians = fieldOfViewInDegrees * 0.5f / 180.0f * 3.14159f;
        float scalingFactor = cos(halfOfFieldOfViewInRadians) / sin(halfOfFieldOfViewInRadians);
        mat4x4 matrix;
        matrix.elements[0 * 4 + 0] = fAspectRatio * scalingFactor;
        matrix.elements[1 * 4 + 1] = scalingFactor;
        matrix.elements[2 * 4 + 2] = fFar / (fFar - fNear);
        matrix.elements[3 * 4 + 2] = (-fFar * fNear) / (fFar - fNear);
        matrix.elements[2 * 4 + 3] = 1.0f;
        matrix.elements[3 * 4 + 3] = 0.0f;
        return matrix;
    }

    bool OnUserUpdate(float elapsedTime) override {
        FillRect(0, 0, ScreenWidth(), ScreenHeight(), olc::BLACK);
        ProcessInput(elapsedTime);
        HandleCameraMoveAndShading(elapsedTime);
        SortTrianglesByAverageZ();
        DrawTriangles();
        return true;
    }

    void ProcessInput(float elapsedTime) {
        const vec3 velocityVector = Vector_Mul(myCameraDirection, 4.0f * elapsedTime);
        if (GetKey(olc::W).bHeld)
            myCameraPosition = Vector_Add(myCameraPosition, velocityVector);
        if (GetKey(olc::A).bHeld) myCameraPosition.x -= 4.0f * elapsedTime;
        if (GetKey(olc::S).bHeld) myCameraPosition = Vector_Sub(myCameraPosition, velocityVector);
        if (GetKey(olc::D).bHeld) myCameraPosition.x += 4.0f * elapsedTime;
        // TODO: Should I normalize camera direction vectors? Given it's rotation, it will be normalized anyway given we are starting at (0, 0, 1)
        if (GetKey(olc::Q).bHeld) {
            const mat4x4 rotationAroundYMatrix = CreateRotationAroundYMatrix(1.0f * elapsedTime);
            myCameraDirection = MultiplyVectorByMatrix(rotationAroundYMatrix, myCameraDirection);
            fYaw += 1.0f * elapsedTime;
        }
        if (GetKey(olc::E).bHeld) {
            const mat4x4 rotationAroundYMatrix = CreateRotationAroundYMatrix(-1.0f * elapsedTime);
            myCameraDirection = NormaliseVector(MultiplyVectorByMatrix(rotationAroundYMatrix, myCameraDirection));
            fYaw -= 1.0f * elapsedTime;
        }
    }

    mat4x4 Matrix_PointAt(const vec3 &pos, const vec3 &target, const vec3 &up) const {
        // Calculate new forward direction
        vec3 newForward = Vector_Sub(target, pos);
        newForward = NormaliseVector(newForward);

        // Calculate new Up direction
        vec3 a = Vector_Mul(newForward, CalculateDotProduct(up, newForward));
        vec3 newUp = Vector_Sub(up, a);
        newUp = NormaliseVector(newUp);

        // New Right direction is easy, its just cross product
        vec3 newRight = CalculateCrossProduct(newUp, newForward);

        // Construct Dimensioning and Translation Matrix	
        mat4x4 matrix;
        matrix.elements[0 * 4 + 0] = newRight.x;
        matrix.elements[0 * 4 + 1] = newRight.y;
        matrix.elements[0 * 4 + 2] = newRight.z;
        matrix.elements[0 * 4 + 3] = 0.0f;
        matrix.elements[1 * 4 + 0] = newUp.x;
        matrix.elements[1 * 4 + 1] = newUp.y;
        matrix.elements[1 * 4 + 2] = newUp.z;
        matrix.elements[1 * 4 + 3] = 0.0f;
        matrix.elements[2 * 4 + 0] = newForward.x;
        matrix.elements[2 * 4 + 1] = newForward.y;
        matrix.elements[2 * 4 + 2] = newForward.z;
        matrix.elements[2 * 4 + 3] = 0.0f;
        matrix.elements[3 * 4 + 0] = pos.x;
        matrix.elements[3 * 4 + 1] = pos.y;
        matrix.elements[3 * 4 + 2] = pos.z;
        matrix.elements[3 * 4 + 3] = 1.0f;
        return matrix;

    }

    mat4x4 Matrix_QuickInverse(mat4x4 &m) const // Only for Rotation/Translation Matrices
    {
        mat4x4 matrix;
        matrix.elements[0 * 4 + 0] = m.elements[0 * 4 + 0];
        matrix.elements[0 * 4 + 1] = m.elements[1 * 4 + 0];
        matrix.elements[0 * 4 + 2] = m.elements[2 * 4 + 0];
        matrix.elements[0 * 4 + 3] = 0.0f;
        matrix.elements[1 * 4 + 0] = m.elements[0 * 4 + 1];
        matrix.elements[1 * 4 + 1] = m.elements[1 * 4 + 1];
        matrix.elements[1 * 4 + 2] = m.elements[2 * 4 + 1];
        matrix.elements[1 * 4 + 3] = 0.0f;
        matrix.elements[2 * 4 + 0] = m.elements[0 * 4 + 2];
        matrix.elements[2 * 4 + 1] = m.elements[1 * 4 + 2];
        matrix.elements[2 * 4 + 2] = m.elements[2 * 4 + 2];
        matrix.elements[2 * 4 + 3] = 0.0f;
        matrix.elements[3 * 4 + 0] = -(m.elements[3 * 4 + 0] * matrix.elements[0 * 4 + 0] +
                                       m.elements[3 * 4 + 1] * matrix.elements[1 * 4 + 0] +
                                       m.elements[3 * 4 + 2] * matrix.elements[2 * 4 + 0]);
        matrix.elements[3 * 4 + 1] = -(m.elements[3 * 4 + 0] * matrix.elements[0 * 4 + 1] +
                                       m.elements[3 * 4 + 1] * matrix.elements[1 * 4 + 1] +
                                       m.elements[3 * 4 + 2] * matrix.elements[2 * 4 + 1]);
        matrix.elements[3 * 4 + 2] = -(m.elements[3 * 4 + 0] * matrix.elements[0 * 4 + 2] +
                                       m.elements[3 * 4 + 1] * matrix.elements[1 * 4 + 2] +
                                       m.elements[3 * 4 + 2] * matrix.elements[2 * 4 + 2]);
        matrix.elements[3 * 4 + 3] = 1.0f;
        return matrix;
    }

    void HandleCameraMoveAndShading(float fElapsedTime) {
        myAngle += fElapsedTime * 0.8f;

        mat4x4 basisTranslation = CreateBasicTranslationMatrix();
        mat4x4 basisTranslation2 = CreateBasicTranslationMatrix2();
        for (const auto &mesh : myMeshes)
            for (const auto &tri : mesh.triangles) {
                triangle translatedToCameraView = MultiplyTriangleByMatrix(basisTranslation2, tri);
                vec3 normal = NormaliseVector(CalculateTriangleNormal(translatedToCameraView));
                if (!IsTriangleLookingAtCamera(translatedToCameraView, normal, myCameraPosition)) continue;
                olc::Pixel shading = CalculateShading(normal);
                trianglesToRaster.push_back({translatedToCameraView.p1, translatedToCameraView.p2,
                                             translatedToCameraView.p3, shading});
            }
    }

    float fYaw{};

    [[nodiscard]] mat4x4 CreateBasicTranslationMatrix2() {
        // Create "Point At" Matrix for camera
        vec3 vUp = {0, 1, 0};
        vec3 vTarget = {0, 0, 1};
        mat4x4 matCameraRot = CreateRotationAroundYMatrix(fYaw);
        
        vTarget = Vector_Add(myCameraPosition, myCameraDirection);
        mat4x4 matCamera = Matrix_PointAt(myCameraPosition, vTarget, vUp);

        // Make view matrix from camera
        return Matrix_QuickInverse(matCamera);
    }
    
    [[nodiscard]] mat4x4 CreateBasicTranslationMatrix() {
        vec3 newDirectionZ = myCameraDirection;
        // TODO: Need to check that direction Y and forward vectors aren't too close otherwise cross product will be (0, 0, 0)
        vec3 tmp = NormaliseVector(CalculateCrossProduct(myDirectionY, newDirectionZ));
        // TODO: Check if order is correct
        vec3 newDirectionY = NormaliseVector(CalculateCrossProduct(newDirectionZ, tmp));
        vec3 newDirectionX = NormaliseVector(CalculateCrossProduct(newDirectionY, newDirectionZ));

        mat4x4 basisTranslation{};
        basisTranslation.elements[0 * 4 + 0] = newDirectionX.x;
        basisTranslation.elements[1 * 4 + 0] = newDirectionX.y;
        basisTranslation.elements[2 * 4 + 0] = newDirectionX.z;
        basisTranslation.elements[3 * 4 + 0] = -CalculateDotProduct(myCameraPosition, newDirectionX);

        basisTranslation.elements[0 * 4 + 1] = newDirectionY.x;
        basisTranslation.elements[1 * 4 + 1] = newDirectionY.y;
        basisTranslation.elements[2 * 4 + 1] = newDirectionY.z;
        basisTranslation.elements[3 * 4 + 1] = -CalculateDotProduct(myCameraPosition, newDirectionY);

        basisTranslation.elements[0 * 4 + 2] = newDirectionZ.x;
        basisTranslation.elements[1 * 4 + 2] = newDirectionZ.y;
        basisTranslation.elements[2 * 4 + 2] = newDirectionZ.z;
        basisTranslation.elements[3 * 4 + 2] = -CalculateDotProduct(myCameraPosition, newDirectionZ);
        basisTranslation.elements[3 * 4 + 3] = 1.0f;
        return basisTranslation;
    }

    static bool IsTriangleLookingAtCamera(const triangle &translated, const vec3 &normal, vec3 &camera) {
        return CalculateDotProduct(normal, Vector_Sub(translated.p1, camera)) < 0.0f;
    }

    void DrawTriangles() {
        for (const auto &tri : trianglesToRaster) {
            triangle projectedTriangle = ProjectTriangleToNormalizedSpace(myProjectionMatrix, tri);
            screenTriangle screenTriangle = TranslateProjectedTriangleToScreenSpace(projectedTriangle);
            DrawTriangle(tri, screenTriangle, tri.color);
        }
        trianglesToRaster.clear();
    }

    void DrawTriangle(const triangle &tri, const screenTriangle &screenTriangle, const olc::Pixel &color) {
        FillTriangle(screenTriangle.p1.w, screenTriangle.p1.h, screenTriangle.p2.w, screenTriangle.p2.h,
                     screenTriangle.p3.w, screenTriangle.p3.h, color);
    }

    static triangle ProjectTriangleToNormalizedSpace(const mat4x4 &projectionMatrix, const triangle &tri) {
        triangle projectedTriangle = MultiplyTriangleByMatrix(projectionMatrix, tri);
        projectedTriangle.p1 = DivideVectorComponents(projectedTriangle.p1, projectedTriangle.p1.w);
        projectedTriangle.p2 = DivideVectorComponents(projectedTriangle.p2, projectedTriangle.p2.w);
        projectedTriangle.p3 = DivideVectorComponents(projectedTriangle.p3, projectedTriangle.p3.w);
        return projectedTriangle;
    }

    static olc::Pixel CalculateShading(const vec3 &normal) {
        float alignmentWithLight = CalculateDotProduct(LightDirection, normal);
        int shadingIntensity = (int) (DarkColor + abs(alignmentWithLight) * LightMaxIntensityDelta);
        auto color = olc::Pixel(shadingIntensity, shadingIntensity, shadingIntensity);
        return color;
    }

    static vec3 CalculateTriangleNormal(const triangle &input) {
        vec3 normal;
        vec3 line1{}, line2{};
        line1.x = input.p2.x - input.p1.x;
        line1.y = input.p2.y - input.p1.y;
        line1.z = input.p2.z - input.p1.z;

        line2.x = input.p3.x - input.p1.x;
        line2.y = input.p3.y - input.p1.y;
        line2.z = input.p3.z - input.p1.z;

        normal = CalculateCrossProduct(line1, line2);
        return normal;
    }

    void SortTrianglesByAverageZ() {
        std::sort(trianglesToRaster.begin(), trianglesToRaster.end(), [](triangle &a, triangle &b) {
            float firstAverageZ = (a.p1.z + a.p2.z + a.p3.z) / 3.0f;
            float secondAverageZ = (b.p1.z + b.p2.z + b.p3.z) / 3.0f;
            return firstAverageZ > secondAverageZ;
        });
    }

private:
    constexpr static float FieldOfView = 135.0f;
    constexpr static float FarthestDepth = 1000.0f;
    constexpr static float CameraDepth = 0.0001f;
    constexpr static float DarkColor = 25.0f;
    constexpr static float LightMaxIntensityDelta = 200.0f;
    constexpr static vec3 LightDirection = {0.0f, 0.0f, -1.0f};

    float myAngle{};
    std::vector<mesh> myMeshes;
    mat4x4 myProjectionMatrix;
    mat4x4 myTranslationMatrix;

    vec3 myCameraPosition{};
    vec3 myCameraDirection = {0, 0, 1};
    const vec3 myDirectionY = {0, 1, 0};

    std::vector<triangle> trianglesToRaster{};

    screenTriangle TranslateProjectedTriangleToScreenSpace(const triangle &normalized) {
        screenVector p1 = TranslateProjectedVertexToScreenSpace(normalized.p1.x, normalized.p1.y);
        screenVector p2 = TranslateProjectedVertexToScreenSpace(normalized.p2.x, normalized.p2.y);
        screenVector p3 = TranslateProjectedVertexToScreenSpace(normalized.p3.x, normalized.p3.y);
        return screenTriangle{p1, p2, p3};
    }

    screenVector TranslateProjectedVertexToScreenSpace(float normalizedWidth, float normalizedHeight) {
        float width = ((normalizedWidth + 1.0f) * 0.5f * (float) ScreenWidth());
        float height = ((normalizedHeight + 1.0f) * 0.5f * (float) ScreenHeight());
        return screenVector{static_cast<int32_t>(width), static_cast<int32_t>(height)};
    }

    static vec3 MultiplyVectorByMatrix(const mat4x4 &matrix, const vec3 &inputTriangle) {
        vec3 v{};
        v.x = inputTriangle.x * matrix.elements[0 * 4 + 0] + inputTriangle.y * matrix.elements[1 * 4 + 0] +
              inputTriangle.z * matrix.elements[2 * 4 + 0] + inputTriangle.w * matrix.elements[3 * 4 + 0];
        v.y = inputTriangle.x * matrix.elements[0 * 4 + 1] + inputTriangle.y * matrix.elements[1 * 4 + 1] +
              inputTriangle.z * matrix.elements[2 * 4 + 1] + inputTriangle.w * matrix.elements[3 * 4 + 1];
        v.z = inputTriangle.x * matrix.elements[0 * 4 + 2] + inputTriangle.y * matrix.elements[1 * 4 + 2] +
              inputTriangle.z * matrix.elements[2 * 4 + 2] + inputTriangle.w * matrix.elements[3 * 4 + 2];
        v.w = inputTriangle.x * matrix.elements[0 * 4 + 3] + inputTriangle.y * matrix.elements[1 * 4 + 3] +
              inputTriangle.z * matrix.elements[2 * 4 + 3] + inputTriangle.w * matrix.elements[3 * 4 + 3];
        return v;
    }

    static triangle MultiplyTriangleByMatrix(const mat4x4 &matrix, const triangle &inputTriangle) {
        vec3 p1 = MultiplyVectorByMatrix(matrix, inputTriangle.p1);
        vec3 p2 = MultiplyVectorByMatrix(matrix, inputTriangle.p2);
        vec3 p3 = MultiplyVectorByMatrix(matrix, inputTriangle.p3);
        return {p1, p2, p3};
    }

    [[maybe_unused]]
    static mat4x4 CreateIdentityMatrix() {
        mat4x4 matrix;
        matrix.elements[0 * 4 + 0] = 1.0f;
        matrix.elements[1 * 4 + 1] = 1.0f;
        matrix.elements[2 * 4 + 2] = 1.0f;
        matrix.elements[3 * 4 + 3] = 1.0f;
        return matrix;
    }

    [[maybe_unused]]
    static mat4x4 CreateRotationAroundXMatrix(float angleInRadians) {
        mat4x4 matrix;
        matrix.elements[0 * 4 + 0] = 1.0f;
        matrix.elements[1 * 4 + 1] = cosf(angleInRadians);
        matrix.elements[1 * 4 + 2] = sinf(angleInRadians);
        matrix.elements[2 * 4 + 1] = -sinf(angleInRadians);
        matrix.elements[2 * 4 + 2] = cosf(angleInRadians);
        matrix.elements[3 * 4 + 3] = 1.0f;
        return matrix;
    }

    [[maybe_unused]]
    static mat4x4 CreateRotationAroundYMatrix(float angleInRadians) {
        mat4x4 matrix;
        matrix.elements[0 * 4 + 0] = cosf(angleInRadians);
        matrix.elements[0 * 4 + 2] = sinf(angleInRadians);
        matrix.elements[2 * 4 + 0] = -sinf(angleInRadians);
        matrix.elements[1 * 4 + 1] = 1.0f;
        matrix.elements[2 * 4 + 2] = cosf(angleInRadians);
        matrix.elements[3 * 4 + 3] = 1.0f;
        return matrix;
    }

    [[maybe_unused]]
    static mat4x4 CreateRotationAroundZMatrix(float angleInRadians) {
        mat4x4 matrix;
        matrix.elements[0 * 4 + 0] = cosf(angleInRadians);
        matrix.elements[0 * 4 + 1] = sinf(angleInRadians);
        matrix.elements[1 * 4 + 0] = -sinf(angleInRadians);
        matrix.elements[1 * 4 + 1] = cosf(angleInRadians);
        matrix.elements[2 * 4 + 2] = 1.0f;
        matrix.elements[3 * 4 + 3] = 1.0f;
        return matrix;
    }


#pragma clang diagnostic push
#pragma ide diagnostic ignored "ConstantParameter"

    static mat4x4 CreateTranslationMatrix(float x, float y, float z) {
        mat4x4 matrix;
        matrix.elements[0 * 4 + 0] = 1.0f;
        matrix.elements[1 * 4 + 1] = 1.0f;
        matrix.elements[2 * 4 + 2] = 1.0f;
        matrix.elements[3 * 4 + 3] = 1.0f;
        matrix.elements[3 * 4 + 0] = x;
        matrix.elements[3 * 4 + 1] = y;
        matrix.elements[3 * 4 + 2] = z;
        return matrix;
    }

#pragma clang diagnostic pop

#pragma clang diagnostic push
#pragma ide diagnostic ignored "UnreachableCallsOfFunction"

    [[maybe_unused]]
    static vec3 Vector_Add(const vec3 &v1, const vec3 &v2) {
        return {v1.x + v2.x, v1.y + v2.y, v1.z + v2.z};
    }

#pragma clang diagnostic pop

    [[maybe_unused]]
    static vec3 Vector_Sub(const vec3 &v1, const vec3 &v2) {
        return {v1.x - v2.x, v1.y - v2.y, v1.z - v2.z};
    }

#pragma clang diagnostic push
#pragma ide diagnostic ignored "UnreachableCallsOfFunction"

    [[maybe_unused]]
    static vec3 Vector_Mul(const vec3 &v1, float k) {
        return {v1.x * k, v1.y * k, v1.z * k};
    }

#pragma clang diagnostic pop

    static vec3 DivideVectorComponents(const vec3 &v1, float k) {
        return {v1.x / k, v1.y / k, v1.z / k};
    }

    static float CalculateDotProduct(const vec3 &v1, const vec3 &v2) {
        return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
    }

    static float Vector_Length(const vec3 &v) {
        return sqrtf(CalculateDotProduct(v, v));
    }

    static vec3 NormaliseVector(const vec3 &v) {
        float l = Vector_Length(v);
        return {v.x / l, v.y / l, v.z / l};
    }

    static vec3 CalculateCrossProduct(const vec3 &v1, const vec3 &v2) {
        vec3 v{};
        v.x = v1.y * v2.z - v1.z * v2.y;
        v.y = v1.z * v2.x - v1.x * v2.z;
        v.z = v1.x * v2.y - v1.y * v2.x;
        return v;
    }

    [[maybe_unused]]
    static vec3 Vector_IntersectPlane(const vec3 &plane_p, vec3 &plane_n, const vec3 &lineStart, const vec3 &lineEnd) {
        plane_n = NormaliseVector(plane_n);
        float plane_d = -CalculateDotProduct(plane_n, plane_p);
        float ad = CalculateDotProduct(lineStart, plane_n);
        float bd = CalculateDotProduct(lineEnd, plane_n);
        float t = (-plane_d - ad) / (bd - ad);
        vec3 lineStartToEnd = Vector_Sub(lineEnd, lineStart);
        vec3 lineToIntersect = Vector_Mul(lineStartToEnd, t);
        return Vector_Add(lineStart, lineToIntersect);
    }
};


int main() {
    olcEngine3D demo;
    if (demo.Construct(1920, 1080, 1, 1) == olc::OK)
        demo.Start();
    return 0;
}