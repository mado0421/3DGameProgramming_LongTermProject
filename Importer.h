#pragma once
class Importer
{
    void ReadMesh();
    void ReadTexture();
    void ReadLinker(const string fileName);
};

struct Triangle
{
    unsigned short  index[3];
};

void CalculateTangentArray(long vertexCount, const XMFLOAT3* vertex, const XMFLOAT3* normal,
    const XMFLOAT2* texcoord, long triangleCount, const Triangle* triangle, XMFLOAT4* tangent)
{
    XMFLOAT3* tan1 = new XMFLOAT3[vertexCount * 2];
    XMFLOAT3* tan2 = tan1 + vertexCount;
    ZeroMemory(tan1, vertexCount * sizeof(XMFLOAT3) * 2);

    for (long a = 0; a < triangleCount; a++)
    {
        long i1 = triangle->index[0];
        long i2 = triangle->index[1];
        long i3 = triangle->index[2];

        const XMFLOAT3& v1 = vertex[i1];
        const XMFLOAT3& v2 = vertex[i2];
        const XMFLOAT3& v3 = vertex[i3];

        const XMFLOAT2& w1 = texcoord[i1];
        const XMFLOAT2& w2 = texcoord[i2];
        const XMFLOAT2& w3 = texcoord[i3];

        float x1 = v2.x - v1.x;
        float x2 = v3.x - v1.x;
        float y1 = v2.y - v1.y;
        float y2 = v3.y - v1.y;
        float z1 = v2.z - v1.z;
        float z2 = v3.z - v1.z;

        float s1 = w2.x - w1.x;
        float s2 = w3.x - w1.x;
        float t1 = w2.y - w1.y;
        float t2 = w3.y - w1.y;

        float r = 1.0F / (s1 * t2 - s2 * t1);
        XMFLOAT3 sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r);
        XMFLOAT3 tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r, (s1 * z2 - s2 * z1) * r);

        tan1[i1] = Vector3::Add(tan1[i1], sdir);
        tan1[i2] = Vector3::Add(tan1[i2], sdir);
        tan1[i3] = Vector3::Add(tan1[i3], sdir);

        tan2[i1] = Vector3::Add(tan1[i1], tdir);
        tan2[i2] = Vector3::Add(tan1[i2], tdir);
        tan2[i3] = Vector3::Add(tan1[i3], tdir);
        
        triangle++;
    }

    for (long a = 0; a < vertexCount; a++)
    {
        const XMFLOAT3& n = normal[a];
        const XMFLOAT3& t = tan1[a];

        

        
        
        // Gram-Schmidt orthogonalize
        //tangent[a] = (t - n * Vector3::DotProduct(n, t)).Normalize();
        XMFLOAT3 temp = Vector3::Normalize(Vector3::ScalarProduct(Vector3::Add(t, Vector3::ScalarProduct(n, -1, true)), Vector3::DotProduct(n, t)));
        tangent[a] = XMFLOAT4(temp.x, temp.y, temp.z, (Vector3::DotProduct(Vector3::CrossProduct(n, t), tan2[a]) < 0.0F) ? -1.0F : 1.0F);

        // Calculate handedness
        //tangent[a].w = (Vector3::DotProduct(Vector3::CrossProduct(n, t), tan2[a]) < 0.0F) ? -1.0F : 1.0F;
    }

    delete[] tan1;
}