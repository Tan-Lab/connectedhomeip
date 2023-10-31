
# Introduction
This is project to build echonetLITE Matter briged-app.
The project was based on the project example/bridge-app/linux

# Build and Run
1. Active Matter environment: [the build guide](guides/BUILDING.md)
2. Run commands
```
    cd examples/bridge-app/echonetlinux
    make
```

# Parameters
| Parameter | Description |
| --- | --- |
| -asyncread | Asynchronous read |
| -asyncwrite | Asynchronous write |
| -get_interval <t> | Set get interval time <t>; Disable by setting <t> =0 |


# Runtime commands
| Command | Description |
| --- | --- |
| R | Factory reset |
| E | Safe Exit|
| e | Show mappedechonetLITE endpoints|
| f | Show echonetLITE endpoints and GET/SET|
| g | Show echonetLITE endpoints and GET/SET data|
| h | Show echonetLITE endpoints|
| j | Show mapped addresses|
| j | Show add addresses|
| c | Show current configs|



# Important notes to sync upsteam
1.  Copy and replace zap files in bridge-common project
2.  In file src/app/clusters/window-covering-server/window-covering-server.cpp
        1. #ifndef EMBER_AF_WINDOW_COVERING_CLUSTER_SERVER_ENDPOINT_COUNT #define
           EMBER_AF_WINDOW_COVERING_CLUSTER_SERVER_ENDPOINT_COUNT (0) #endif
        2. Correct HasFeature() function (bug from the SDK):
           emberAfWindowCoveringClusterUpOrOpenCallback() and
           emberAfWindowCoveringClusterDownOrCloseCallback() if
           (HasFeature(endpoint, Feature::kPositionAwareLift) || true)

    ```cpp
                 if (delegate)
            {
             if (HasFeature(endpoint, Feature::kPositionAwareLift)|| true)
             {
                 LogErrorOnFailure(delegate->HandleMovement(WindowCoveringType::Lift,100));
             }

             if (HasFeature(endpoint, Feature::kPositionAwareTilt)|| true)
             {
                 LogErrorOnFailure(delegate->HandleMovement(WindowCoveringType::Tilt,100));
             }
            }
    ```

    ```cpp

    if (delegate) { if (HasFeature(endpoint, Feature::kPositionAwareLift)||
    true) {
    LogErrorOnFailure(delegate->HandleMovement(WindowCoveringType::Lift,0)); }

            if (HasFeature(endpoint, Feature::kPositionAwareTilt)|| true)
            {
                LogErrorOnFailure(delegate->HandleMovement(WindowCoveringType::Tilt,0));
            }
        }
    ```

3.  In file src/app/clusters/window-covering-server/window-covering-delegate.h
    add int openPercent variable virtual CHIP_ERROR
    HandleMovement(WindowCoveringType type, int openPercent) = 0;
4.  In file src/app/clusters/window-covering-server/window-covering-server.cpp
    ```cpp
    delegate->HandleMovement(WindowCoveringType::Lift,100)
    delegate->HandleMovement(WindowCoveringType::Tilt,100)
    delegate->HandleMovement(WindowCoveringType::Lift,0)
    delegate->HandleMovement(WindowCoveringType::Tilt,0)
    delegate->HandleMovement(WindowCoveringType::Lift,liftValue)
    delegate->HandleMovement(WindowCoveringType::Lift,percent100ths)
    delegate->HandleMovement(WindowCoveringType::Tilt,tiltValue)
    delegate->HandleMovement(WindowCoveringType::Tilt,percent100ths)
    ```


# Tips to manually edit the zap files
## bridge-app.matter
1. Search for "endpoint 0" in source .matter file,. 
2. Search for "endpoint 1" in the target .matter file. Put the content under the "endpoint 1" tag.
Sometime the desired infomation is listed near "endpoint 2".
## bridge-app.zap
1. Search for keyword. For example "Window Covering"
2. Put it under "endpointTypes.clusters". Note: COPY TWO OF THEM 
