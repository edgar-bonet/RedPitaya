#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>
#include <asio.hpp>

#include <Oscilloscope.h>
#include <StreamingManager.h>

//#define DISABLE_OSC

class CStreamingApplication
{
public:
    CStreamingApplication(CStreamingManager::Ptr _StreamingManager, COscilloscope::Ptr _osc_ch,unsigned short _resolution,int _oscRate, int _channels);
    ~CStreamingApplication();
    void run();
    void runNonBlock();
    bool stop();
private:
    int m_PerformanceCounterPeriod = 10;

    COscilloscope::Ptr m_Osc_ch;
    CStreamingManager::Ptr m_StreamingManager;
    std::thread m_OscThread;
    std::thread m_SocketThread;
    std::mutex mtx;
    std::atomic_flag m_OscThreadRun = ATOMIC_FLAG_INIT;
    std::atomic_int m_ReadyToPass;
    bool            m_isRun;
    static_assert(ATOMIC_INT_LOCK_FREE == 2,"this implementation does not guarantee that std::atomic<int> is always lock free.");

    asio::io_service m_Ios;
    unsigned short m_Resolution;

    void *m_WriteBuffer_ch1[2];
    void *m_WriteBuffer_ch2[2];
    size_t m_size_ch1[2];
    size_t m_size_ch2[2];
    std::atomic_int  m_was_send[2];
    uint64_t         m_lostRate[2];
    int              m_oscRate;
    int              m_channels;
    std::atomic_int  m_bufferIndex;

    uint8_t val;
    asio::steady_timer m_Timer;
    uintmax_t m_BytesCount;

    void oscWorker();
    bool passCh(int _bufferIndex, size_t &_size1,size_t &_size2);
    int  oscNotify(uint64_t _lostRate, uint32_t _oscRate,const void *_buffer_ch1, size_t _size_ch1,const void *_buffer_ch2, size_t _size_ch2);
    void signalHandler(const asio::error_code &_error, int _signalNumber);
};