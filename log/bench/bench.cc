#include "../mylogs/logs.hpp"
#include <chrono>

namespace log
{
    void bench(const std::string &loger_name, size_t thread_num,
               size_t msglen, size_t msg_count)
    {
        Logger::ptr lp = getLogger(loger_name);
        if (lp.get() == nullptr)
            return;
        std::string msg(msglen, '1');
        size_t msg_count_per_thread = msg_count / thread_num;
        std::vector<double> cost_time(thread_num);
        std::vector<std::thread> threads;
        std::cout << "输⼊线程数量: " << thread_num << std::endl;
        std::cout << "输出⽇志数量: " << msg_count << std::endl;
        std::cout << "输出⽇志⼤⼩: " << msglen * msg_count / 1024 << "KB" << std::endl;
        for (int i = 0; i < thread_num; i++)
        {
            threads.emplace_back([&, i]()
                                 {
 auto start = std::chrono::high_resolution_clock::now();
 for(size_t j = 0; j < msg_count_per_thread; j++) {
 lp->fatal("%s", msg.c_str());
 }
 auto end = std::chrono::high_resolution_clock::now();
 auto cost=std::chrono::duration_cast<std::chrono::duration<double>>
(end-start);
 cost_time[i] = cost.count();
 auto avg = msg_count_per_thread / cost_time[i];
 std::cout << "线程" << i << "耗时: " << cost.count() << "s";
 std::cout << " 平均：" << (size_t)avg << "/s\n"; });
        }
        for (auto &thr : threads)
        {
            thr.join();
        }
        double max_cost = 0;
        for (auto cost : cost_time)
            max_cost = max_cost < cost ? cost : max_cost;
        std::cout << "总消耗时间: " << max_cost << std::endl;
        std::cout << "平均每秒输出: " << (size_t)(msg_count / max_cost) << std::endl;
    }

    void sync_bench_thread_log(size_t thread_count, size_t msg_count, size_t msglen)
    {
        static int num = 1;
        std::string logger_name = "sync_bench_logger" + std::to_string(num++);
        INFO("************************************************");
        INFO("同步⽇志测试: %d threads, %d messages", thread_count, msg_count);
        log::GlobalLoggerBuilder::ptr lbp(new log::GlobalLoggerBuilder);
        lbp->buildLoggerName(logger_name);
        lbp->buildFormatter("%m%n");
        lbp->buildSink<log::FileSink>("./logs/sync.log");
        lbp->buildLoggerType(log::LoggerType::LOGGER_SYNC);
        lbp->build();
        log::bench(logger_name, thread_count, msglen, msg_count);
        INFO("************************************************");
    }

    void async_bench_thread_log(size_t thread_count, size_t msg_count, size_t msglen)
    {
        static int num = 1;
        std::string logger_name = "async_bench_logger" + std::to_string(num++);
        INFO("************************************************");
        INFO("异步⽇志测试: %d threads, %d messages", thread_count, msg_count);
        log::GlobalLoggerBuilder::ptr lbp(new log::GlobalLoggerBuilder);
        lbp->buildLoggerName(logger_name);
        lbp->buildFormatter("%m");
        lbp->buildSink<log::FileSink>("./logs/async.log");
        lbp->buildLoggerType(log::LoggerType::LOGGER_ASYNC);
        lbp->build();
        log::bench(logger_name, thread_count, msglen, msg_count);
        INFO("************************************************");
    }

    void bench_test()
    {
        // 同步写⽇志
        sync_bench_thread_log(1, 10000000, 100);
        sync_bench_thread_log(5, 10000000, 100);
        /*异步⽇志输出，为了避免因为等待落地影响时间所以⽇志数量降低为⼩于缓冲区⼤⼩进⾏测试*/
        async_bench_thread_log(1, 10000000, 100);
        async_bench_thread_log(5, 10000000, 100);
    }
};
int main(int argc, char *argv[])
{
    log::bench_test();
    return 0;
}
