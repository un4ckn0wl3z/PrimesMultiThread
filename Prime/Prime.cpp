#include <Windows.h>
#include <stdio.h>
#include <memory>
#include <math.h>

typedef struct _THREAD_DATA {
    int first;
    int last;
    int count;
} THREAD_DATA, *PTHREAD_DATA;


bool IsPrime(int num)
{
    int limit = (int)sqrt(num);
    for (size_t i = 2; i <= limit; i++)
    {
        if (num % i == 0) return false;
    }

    return true;
}


DWORD WINAPI CountPrimes(PVOID param)
{
    auto data = (PTHREAD_DATA)param;
    int count = 0;

    for (size_t i = data->first ; i <= data->last; i++)
    {
        if (IsPrime(i)) 
        {
            count++;
        }
    }
    
    data->count = count;
    return 0;

}

int CalcPrimes(int first, int last, int threads)
{
    int chunk = (last - first + 1) / threads;
    // HANDLE* handles = (HANDLE*)malloc(sizeof(HANDLE) * threads);

    auto handles = std::make_unique<HANDLE[]>(threads);
    auto data = std::make_unique<THREAD_DATA[]>(threads);

    for (size_t i = 0; i < threads; i++)
    {
        data[i].first = first + i * chunk;
        data[i].last = i == threads - 1 ? last : data[i].first + chunk - 1;
        handles[i] = CreateThread(nullptr, 0, CountPrimes, &data[i], 0, nullptr);
    }

    WaitForMultipleObjects(threads, handles.get(), TRUE, INFINITE);

    // free(handles);

    int total = 0;
    for (size_t i = 0; i < threads; i++)
    {
        total += data[i].count;
        CloseHandle(handles[i]);
    }

    return total;

}


int main(int argc, const char* argv[])
{
    int threads = 1;
    
    if (argc > 1) {
        threads = atoi(argv[1]);
    }
    printf("Working with %d threads...\n", threads);
    auto start = GetTickCount64();
    int count = CalcPrimes(3, 20000000, threads);
    auto end = GetTickCount64();

    printf("Total: %d Elapsed: %d\n", count, (DWORD)(end - start));

    return 0;
}
 
