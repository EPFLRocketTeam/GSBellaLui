#ifndef MAINWORKER_H
#define MAINWORKER_H

#include <QtWidgets/QWidget>
#include <DataStructures/datastructs.h>
#include <DataHandlers/TelemetryHandler.h>
#include <qcustomplot.h>
#include <chrono>
#include "GraphFeature.h"
#include "FileLogger.h"

class GSMainwindow;
using namespace std;

enum SoftwareMode {
    REAL_TIME, REPLAY
};

class Worker : public QObject {
Q_OBJECT

public:
    explicit Worker(GSMainwindow *);

    ~Worker() override;

    void mainRoutine();

    void defineCurrentRunningMode(const SoftwareMode &mode, const std::string &);


public slots:

    void run();

    void emitAllStatuses();

    void updateLoggingStatus();

    void updatePlaybackSpeed(double);

    void resetPlayback();

    void reversePlayback(bool);

    void setReplayMode(bool);

signals:

    void loggingStatusReady(bool);

    void telemetryReady(TelemetryReading);

    void graphDataReady(QVector<QCPGraphData> &, GraphFeature);

    void newEventsReady(vector<RocketEvent> &);

    void points3DReady(QVector<QVector3D> &);

    void linkStatusReady(HandlerStatus);

    void groundStatusReady(float, float);

    void dummySignal();


private:
    bool loggingEnabled_;
    bool replayMode_;
    std::atomic<bool> updateHandler_;

    void checkLinkStatuses();

    void displayMostRecentTelemetry(TelemetryReading);


    unique_ptr<GSMainwindow> mainWidget_;
    unique_ptr<TelemetryHandler> telemetryHandler_;
    unique_ptr<TelemetryHandler> newHandler_;
    FileLogger telemetryLogger;
    FileLogger eventLogger;
    chrono::system_clock::time_point lastUIupdate;
    chrono::system_clock::time_point lastIteration;
    chrono::system_clock::time_point timeOfLastLinkCheck;
    chrono::system_clock::time_point timeOfLastReceivedTelemetry;
    long long int millisBetweenLastTwoPackets;

    QVector<QCPGraphData> extractGraphData(vector<TelemetryReading> &, QCPGraphData (*)(TelemetryReading));
};

#endif // WORKER_H
