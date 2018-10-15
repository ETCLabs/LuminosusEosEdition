#ifndef SCENEBLOCKINTERFACE_H
#define SCENEBLOCKINTERFACE_H

class SceneData;

class SceneBlockInterface {

public:

    virtual ~SceneBlockInterface() {}

    virtual SceneData getSceneData() = 0;
    virtual void setSceneData(const SceneData& data) = 0;

    virtual int getSceneGroup() = 0;
    virtual void setSceneGroup(int value) = 0;


};

#endif // SCENEBLOCKINTERFACE_H
