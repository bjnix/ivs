
Assimp::Importer importer;
const aiScene *scene = importer.ReadFile(filename,aiProcessPreset_TargetRealtime_Fast);//aiProcessPreset_TargetRealtime_Fast has the configs you'll need
 
aiMesh *mesh = scene->mMeshes[0]; //assuming you only want the first mesh

float *vertexArray;
float *normalArray;
float *uvArray;
 
int numVerts;

numVerts = mesh->mNumFaces*3;
 
vertexArray = new float[mesh->mNumFaces*3*3];
normalArray = new float[mesh->mNumFaces*3*3];
uvArray = new float[mesh->mNumFaces*3*2];
 
for(unsigned int i=0;i<mesh->mNumFaces;i++)
{
const aiFace& face = mesh->mFaces[i];
 
for(int j=0;j<3;j++)
{
aiVector3D uv = mesh->mTextureCoords[0][face.mIndices[j]];
memcpy(uvArray,&uv,sizeof(float)*2);
uvArray+=2;
 
aiVector3D normal = mesh->mNormals[face.mIndices[j]];
memcpy(normalArray,&normal,sizeof(float)*3);
normalArray+=3;
 
aiVector3D pos = mesh->mVertices[face.mIndices[j]];
memcpy(vertexArray,&pos,sizeof(float)*3);
vertexArray+=3;
}
}
 
uvArray-=mesh->mNumFaces*3*2;
normalArray-=mesh->mNumFaces*3*3;
vertexArray-=mesh->mNumFaces*3*3;

