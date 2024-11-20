# CPU Indirect Drawing
One of the major insights of OpenGL 4.x is the observation that the call

```
void glDrawElementsInstancedBaseVertexBaseInstance(GLenum mode,  
    GLsizei count,
 	GLenum type,
 	void *indices,
 	GLsizei instancecount,
 	GLint basevertex,
 	GLuint baseinstance);
```

is isomorphic to the data in the struct

```
typedef struct {
        uint  count;
        uint  instanceCount;
        uint  firstIndex;
        int  baseVertex;
        uint  baseInstance;
    } DrawElementsIndirectCommand;
```

This has the implication that drawing parameters can be recorded to a buffer and then directly consumed by the GPU. In the next tutorial we'll see the true power: the GPU can record draw commands for itself. For now, we'll practice the technique on the CPU to get it stable.

## In Practice
First, let's define a struct to hold the draw commands.

```
struct DrawCommand {
    uint32_t indexCount = 0;
    uint32_t instanceCount = 0;
    uint32_t firstIndex = 0;
    uint32_t baseVertex = 0;
    uint32_t baseInstance = 0;
};
```

One curiousity here is baseVertex. Strangely, firstIndex and baseVertex seem to do the same thing, they offset the index used in a drawcall. We can ignore it.

Our renderer has specified that it will load 14 unique model types, and we're using instanced rendering, so we'll have 14 draw commands, one for each object type.
```
//Indirect Drawing
uint32_t indirectBuffer;
DrawCommand drawCommands[objectTypeCount];
```

Because the first indices and index counts are stored in draw commands, we don't need to store them separately anymore. They can be directly recorded during model loading.
```
void RenderSystem::build_models() {

    MeshFactory meshFactory;
    for (int i = 0; i < objectTypeCount; ++i) {
        meshFactory.consume(i);
        textures[i] = meshFactory.load_2d_material(imageNames[i]);
        textureHandles[i] = glGetTextureHandleARB(textures[i]);

        drawCommands[i].firstIndex = meshFactory.firstIndices[i];
        drawCommands[i].indexCount = meshFactory.indexCounts[i];
        AABBs[i] = meshFactory.AABBs[i];
    }
    mesh = meshFactory.finalize();

    glCreateBuffers(1, &textureBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, textureBuffer);
    glBufferStorage(GL_SHADER_STORAGE_BUFFER, objectTypeCount * sizeof(GLuint64), textureHandles, 0);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, textureBuffer);

    for (GLuint64 handle : textureHandles) {
        glMakeTextureHandleResidentARB(handle);
    }
}
```
After loading models, we can create the buffer to store the draw commands.
```
glGenBuffers(1, &indirectBuffer);
glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffer);
glBufferStorage(GL_DRAW_INDIRECT_BUFFER, objectTypeCount * sizeof(DrawCommand), 
    drawCommands, GL_DYNAMIC_STORAGE_BIT);
```

Now the update splits into two steps, first we look through the scene, determine what's visible and update our draw commands, then we actually draw it.

```
void parse_scene();
void draw_scene();
```

## Parse Scene
Before parsing the scene, we flush out any junk data.
```
void RenderSystem::parse_scene() {

    for (size_t i = 0; i < objectTypeCount; ++i) {
        drawCommands[i].instanceCount = 0;
    }
}
```

Then for every renderable object, we determine its visibility. If the object is visible, we record its model transform in the appropriate region of the transform buffer, and increment the instance count for its draw command.

```
for (size_t i = 0; i < componentRegistry.renderables.entities.size(); ++i) {
    uint32_t& id = componentRegistry.renderables.entities[i];
    RenderComponent& object = componentRegistry.renderables.components[i];
    TransformComponent& transform = componentRegistry.transforms.get_component(id);
    glm::mat4 model = build_model_transform(transform);

    uint32_t objectType = static_cast<uint32_t>(object.objectType);

    if (!cameraSystem->visible(AABBs[objectType], model)) {
        continue;
    }


    modelTransforms[objectOffsets[objectType] + drawCommands[objectType].instanceCount++] = model;
}
```

Then we can upload both the model transforms and draw commands, ready to go!

```
glBindBuffer(GL_SHADER_STORAGE_BUFFER, modelBuffer);
glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, maxObjectCount * sizeof(glm::mat4), modelTransforms.data());

glBufferSubData(GL_DRAW_INDIRECT_BUFFER, 0, objectTypeCount * sizeof(DrawCommand), drawCommands);
```

## Shaders
Before we draw the scene, we should alter our shaders a little. So far we've been using a uniform to pass through the object type, but indirect drawing gives us something better, an inbuilt variable which counts which indirect draw call we're currently on. It's like indirect drawing was made for this workflow!

vertex:
```
// ...

out vec2 fragmentTexCoord;
out vec3 fragmentNormal;
out flat int samplerIndex;

// ...

void main()
{
    samplerIndex = gl_DrawID;
    mat4 model = modelTransforms[objectOffsets[gl_DrawID] + gl_InstanceID];
    // ...
}
```

fragment:
```
// ...

in vec2 fragmentTexCoord;
in vec3 fragmentNormal;
in flat int samplerIndex;

// ...

void main()
{
    sampler2D tex = textures[samplerIndex];
    // ...
}
```

## Draw Scene
Finally we get to the drawing function, simple and elegant.
```
void RenderSystem::draw_scene() {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (void*)0, objectTypeCount, 0);

    glfwSwapBuffers(window);
}
```