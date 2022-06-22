// QTM Connect For Unreal. Copyright 2018-2022 Qualisys
//
#pragma once

#include "ILiveLinkSource.h"
#include "MessageEndpoint.h"
#include "IMessageContext.h"
#include "HAL/ThreadSafeBool.h"
#include "HAL/RunnableThread.h"
#include "HAL/Runnable.h"

#include "Sockets.h"
#include "SocketSubsystem.h"

#if PLATFORM_WINDOWS
#include "Windows/AllowWindowsPlatformTypes.h"
#include "Windows/AllowWindowsPlatformAtomics.h"
#endif
#include "RTProtocol.h"
#if PLATFORM_WINDOWS
#include "Windows/HideWindowsPlatformAtomics.h"
#include "Windows/HideWindowsPlatformTypes.h"
#endif

DECLARE_LOG_CATEGORY_EXTERN(QTMConnectLiveLinkLog,Log,All);

class ILiveLinkClient;


class QTMCONNECTLIVELINK_API FCRTProtocalWrapper:public CRTProtocol, public TSharedFromThis<FCRTProtocalWrapper>{
public:
	FCRTProtocalWrapper():CRTProtocol(){};
};


class QTMCONNECTLIVELINK_API QTMConnectLiveLinkSettings
{
public:
	QTMConnectLiveLinkSettings() : IpAddress("127.0.0.1"), AutoDiscover(false), Stream3d(false), Stream6d(true), StreamSkeleton(true), StreamForce(false), StreamRate("All Frames"), FrequencyValue(0)
	{
	}

    static QTMConnectLiveLinkSettings FromString(const FString& settingsString);
    FString ToString() const;

    FString IpAddress;
	bool AutoDiscover;
	bool Stream3d;
	bool Stream6d;
	bool StreamSkeleton;
    bool StreamForce;
    FString StreamRate;
    uint32 FrequencyValue;
    static const TArray<FString> STREAMRATES;
};

class QTMCONNECTLIVELINK_API FQTMConnectLiveLinkSource : public ILiveLinkSource, public FRunnable
{
public:

	FQTMConnectLiveLinkSource(const QTMConnectLiveLinkSettings& settings);

	virtual ~FQTMConnectLiveLinkSource();

	// Begin ILiveLinkSource Interface
	
	virtual void ReceiveClient(ILiveLinkClient* InClient, FGuid InSourceGuid) override;

	virtual bool IsSourceStillValid() const override;

	virtual bool RequestSourceShutdown() override;

	virtual FText GetSourceType() const override { return SourceType; };
	virtual FText GetSourceMachineName() const override { return SourceMachineName; }
	virtual FText GetSourceStatus() const override { return SourceStatus; }

	// End ILiveLinkSource Interface

	// Begin FRunnable Interface

	virtual bool Init() override { return true; }
	virtual uint32 Run() override;

    void CreateLiveLinkSubjects();

    void ClearSubjects();

    void Start();
	virtual void Stop() override;
	virtual void Exit() override { }

	// End FRunnable Interface

	void HandleReceivedData(TSharedPtr<TArray<uint8>, ESPMode::ThreadSafe> ReceivedData);

private:
	void DisconnectFromQTM();

private:
    QTMConnectLiveLinkSettings Settings;

	ILiveLinkClient* Client;

	// Our identifier in LiveLink
	FGuid SourceGuid;

	FMessageAddress ConnectionAddress;

	FText SourceType;
	FText SourceMachineName;
	FText SourceStatus;
	
	// Threadsafe Bool for terminating the main thread loop
	FThreadSafeBool Stopping;
	//
	// Thread to run socket operations on
	FRunnableThread* Thread;
	//
	//// Name of the sockets thread
	FString ThreadName;

	// List of subjects we've already encountered
    TArray<FLiveLinkSubjectKey> EncounteredSubjects;

    TSharedPtr<FCRTProtocalWrapper> mRTProtocol;

    TMap<uint32, FString> mForceIdToName;
};
