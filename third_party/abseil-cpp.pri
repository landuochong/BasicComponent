HEADERS += \
        $$PWD/abseil-cpp/absl/algorithm/algorithm.h \
        $$PWD/abseil-cpp/absl/algorithm/container.h \
        $$PWD/abseil-cpp/absl/debugging/stacktrace.h \
        $$PWD/abseil-cpp/absl/debugging/symbolize.h \
        $$PWD/abseil-cpp/absl/memory/memory.h \
        $$PWD/abseil-cpp/absl/meta/type_traits.h \
        $$PWD/abseil-cpp/absl/time/civil_time.h \
        $$PWD/abseil-cpp/absl/time/clock.h \
        $$PWD/abseil-cpp/absl/time/internal/cctz/include/cctz/civil_time.h \
        $$PWD/abseil-cpp/absl/time/internal/cctz/include/cctz/civil_time_detail.h \
        $$PWD/abseil-cpp/absl/time/internal/cctz/include/cctz/time_zone.h \
        $$PWD/abseil-cpp/absl/time/internal/cctz/include/cctz/zone_info_source.h \
        $$PWD/abseil-cpp/absl/time/internal/cctz/src/time_zone_fixed.h \
        $$PWD/abseil-cpp/absl/time/internal/cctz/src/time_zone_if.h \
        $$PWD/abseil-cpp/absl/time/internal/cctz/src/time_zone_impl.h \
        $$PWD/abseil-cpp/absl/time/internal/cctz/src/time_zone_info.h \
        $$PWD/abseil-cpp/absl/time/internal/cctz/src/time_zone_libc.h \
        $$PWD/abseil-cpp/absl/time/internal/cctz/src/time_zone_posix.h \
        $$PWD/abseil-cpp/absl/time/internal/cctz/src/tzfile.h \
        $$PWD/abseil-cpp/absl/time/time.h \
        $$PWD/abseil-cpp/absl/utility/utility.h \
        $$PWD/abseil-cpp/absl/profiling/internal/exponential_biased.h \
        $$PWD/abseil-cpp/absl/profiling/internal/periodic_sampler.h \
        $$PWD/abseil-cpp/absl/profiling/internal/sample_recorder.h \
        $$PWD/abseil-cpp/absl/random/*.h \
        $$PWD/abseil-cpp/absl/random/internal/*.h \
        $$PWD/abseil-cpp/absl/status/*.h \
        $$PWD/abseil-cpp/absl/status/internal/*.h \
        $$PWD/abseil-cpp/absl/crc/crc32c.h \
        $$PWD/abseil-cpp/absl/crc/internal/*.h \
        $$PWD/abseil-cpp/absl/flags/*.h \
        $$PWD/abseil-cpp/absl/flags/internal/*.h \
        $$PWD/abseil-cpp/absl/synchronization/*.h \
        $$PWD/abseil-cpp/absl/synchronization/internal/*.h \
        $$PWD/abseil-cpp/absl/functional/*.h \
        $$PWD/abseil-cpp/absl/functional/internal/*.h \
        $$PWD/abseil-cpp/absl/cleanup/cleanup.h \
        $$PWD/abseil-cpp/absl/cleanup/internal/cleanup.h \
        $$PWD/abseil-cpp/absl/container/*.h \
        $$PWD/abseil-cpp/absl/container/internal/*.h \
        $$PWD/abseil-cpp/absl/hash/hash.h \
        $$PWD/abseil-cpp/absl/hash/internal/city.h \
        $$PWD/abseil-cpp/absl/hash/internal/hash.h \
        $$PWD/abseil-cpp/absl/hash/internal/low_level_hash.h \
        $$PWD/abseil-cpp/absl/hash/internal/spy_hash_state.h \
        $$PWD/abseil-cpp/absl/log/*.h \
        $$PWD/abseil-cpp/absl/log/internal/*.h \
        $$PWD/abseil-cpp/absl/types/*.h \
        $$PWD/abseil-cpp/absl/types/internal/*.h \
        $$PWD/abseil-cpp/absl/numeric/*.h \
        $$PWD/abseil-cpp/absl/numeric/internal/*.h \
        $$PWD/abseil-cpp/absl/base/*.h \
        $$PWD/abseil-cpp/absl/base/internal/*.h

SOURCES += \
#    $$PWD/abseil-cpp/absl/algorithm/equal_benchmark.cc \
    $$PWD/abseil-cpp/absl/debugging/stacktrace.cc \
    $$PWD/abseil-cpp/absl/debugging/symbolize.cc \
    $$PWD/abseil-cpp/absl/status/status.cc \
    $$PWD/abseil-cpp/absl/status/status_payload_printer.cc \
    $$PWD/abseil-cpp/absl/status/statusor.cc \
    $$PWD/abseil-cpp/absl/profiling/internal/exponential_biased.cc \
    $$PWD/abseil-cpp/absl/profiling/internal/periodic_sampler.cc \
#    $$PWD/abseil-cpp/absl/profiling/internal/periodic_sampler_benchmark.cc \
#    $$PWD/abseil-cpp/absl/random/benchmarks.cc \
    $$PWD/abseil-cpp/absl/random/discrete_distribution.cc \
    $$PWD/abseil-cpp/absl/random/gaussian_distribution.cc \
    $$PWD/abseil-cpp/absl/random/seed_gen_exception.cc \
    $$PWD/abseil-cpp/absl/random/seed_sequences.cc \
    $$PWD/abseil-cpp/absl/random/internal/chi_square.cc \
    $$PWD/abseil-cpp/absl/random/internal/gaussian_distribution_gentables.cc \
#    $$PWD/abseil-cpp/absl/random/internal/nanobenchmark.cc \
    $$PWD/abseil-cpp/absl/random/internal/pool_urbg.cc \
    $$PWD/abseil-cpp/absl/random/internal/randen.cc \
#    $$PWD/abseil-cpp/absl/random/internal/randen_benchmarks.cc \
    $$PWD/abseil-cpp/absl/random/internal/randen_detect.cc \
    $$PWD/abseil-cpp/absl/random/internal/randen_hwaes.cc \
    $$PWD/abseil-cpp/absl/random/internal/randen_round_keys.cc \
    $$PWD/abseil-cpp/absl/random/internal/randen_slow.cc \
    $$PWD/abseil-cpp/absl/random/internal/seed_material.cc \
    $$PWD/abseil-cpp/absl/crc/crc32c.cc \
#    $$PWD/abseil-cpp/absl/crc/crc32c_benchmark.cc \
    $$PWD/abseil-cpp/absl/crc/internal/cpu_detect.cc \
    $$PWD/abseil-cpp/absl/crc/internal/crc.cc \
    $$PWD/abseil-cpp/absl/crc/internal/crc_cord_state.cc \
    $$PWD/abseil-cpp/absl/crc/internal/crc_memcpy_fallback.cc \
    $$PWD/abseil-cpp/absl/crc/internal/crc_memcpy_x86_64.cc \
    $$PWD/abseil-cpp/absl/crc/internal/crc_non_temporal_memcpy.cc \
    $$PWD/abseil-cpp/absl/crc/internal/crc_x86_arm_combined.cc \
    $$PWD/abseil-cpp/absl/flags/commandlineflag.cc \
    $$PWD/abseil-cpp/absl/flags/flag.cc \
#    $$PWD/abseil-cpp/absl/flags/flag_benchmark.cc \
    $$PWD/abseil-cpp/absl/flags/marshalling.cc \
    $$PWD/abseil-cpp/absl/flags/parse.cc \
    $$PWD/abseil-cpp/absl/flags/reflection.cc \
    $$PWD/abseil-cpp/absl/flags/usage.cc \
    $$PWD/abseil-cpp/absl/flags/usage_config.cc \
    $$PWD/abseil-cpp/absl/flags/internal/commandlineflag.cc \
    $$PWD/abseil-cpp/absl/flags/internal/flag.cc \
    $$PWD/abseil-cpp/absl/flags/internal/private_handle_accessor.cc \
    $$PWD/abseil-cpp/absl/flags/internal/program_name.cc \
    $$PWD/abseil-cpp/absl/flags/internal/usage.cc \
    $$PWD/abseil-cpp/absl/synchronization/barrier.cc \
    $$PWD/abseil-cpp/absl/synchronization/blocking_counter.cc \
#    $$PWD/abseil-cpp/absl/synchronization/blocking_counter_benchmark.cc \
    $$PWD/abseil-cpp/absl/synchronization/mutex.cc \
#    $$PWD/abseil-cpp/absl/synchronization/mutex_benchmark.cc \
    $$PWD/abseil-cpp/absl/synchronization/notification.cc \
    $$PWD/abseil-cpp/absl/synchronization/internal/create_thread_identity.cc \
    $$PWD/abseil-cpp/absl/synchronization/internal/graphcycles.cc \
#    $$PWD/abseil-cpp/absl/synchronization/internal/graphcycles_benchmark.cc \
    $$PWD/abseil-cpp/absl/synchronization/internal/per_thread_sem.cc \
    $$PWD/abseil-cpp/absl/synchronization/internal/waiter.cc \
#    $$PWD/abseil-cpp/absl/functional/function_type_benchmark.cc \
    $$PWD/abseil-cpp/absl/base/log_severity.cc \
    $$PWD/abseil-cpp/absl/base/internal/cycleclock.cc \
    $$PWD/abseil-cpp/absl/base/internal/low_level_alloc.cc \
    $$PWD/abseil-cpp/absl/base/internal/raw_logging.cc \
    $$PWD/abseil-cpp/absl/base/internal/scoped_set_env.cc \
    $$PWD/abseil-cpp/absl/base/internal/spinlock.cc \
#    $$PWD/abseil-cpp/absl/base/internal/spinlock_benchmark.cc \
    $$PWD/abseil-cpp/absl/base/internal/spinlock_wait.cc \
    $$PWD/abseil-cpp/absl/base/internal/strerror.cc \
#    $$PWD/abseil-cpp/absl/base/internal/strerror_benchmark.cc \
    $$PWD/abseil-cpp/absl/base/internal/sysinfo.cc \
    $$PWD/abseil-cpp/absl/base/internal/thread_identity.cc \
#    $$PWD/abseil-cpp/absl/base/internal/thread_identity_benchmark.cc \
    $$PWD/abseil-cpp/absl/base/internal/throw_delegate.cc \
    $$PWD/abseil-cpp/absl/base/internal/unscaledcycleclock.cc \
#    $$PWD/abseil-cpp/absl/hash/hash_benchmark.cc \
    $$PWD/abseil-cpp/absl/hash/internal/city.cc \
    $$PWD/abseil-cpp/absl/hash/internal/hash.cc \
    $$PWD/abseil-cpp/absl/hash/internal/low_level_hash.cc \
    $$PWD/abseil-cpp/absl/hash/internal/print_hash_of.cc \
    $$PWD/abseil-cpp/absl/time/civil_time.cc \
    $$PWD/abseil-cpp/absl/time/clock.cc \
    $$PWD/abseil-cpp/absl/time/duration.cc \
    $$PWD/abseil-cpp/absl/time/format.cc \
    $$PWD/abseil-cpp/absl/time/internal/cctz/src/civil_time_detail.cc \
    $$PWD/abseil-cpp/absl/time/internal/cctz/src/time_zone_fixed.cc \
    $$PWD/abseil-cpp/absl/time/internal/cctz/src/time_zone_format.cc \
    $$PWD/abseil-cpp/absl/time/internal/cctz/src/time_zone_if.cc \
    $$PWD/abseil-cpp/absl/time/internal/cctz/src/time_zone_impl.cc \
    $$PWD/abseil-cpp/absl/time/internal/cctz/src/time_zone_info.cc \
    $$PWD/abseil-cpp/absl/time/internal/cctz/src/time_zone_libc.cc \
    $$PWD/abseil-cpp/absl/time/internal/cctz/src/time_zone_lookup.cc \
    $$PWD/abseil-cpp/absl/time/internal/cctz/src/time_zone_posix.cc \
    $$PWD/abseil-cpp/absl/time/internal/cctz/src/zone_info_source.cc \
    $$PWD/abseil-cpp/absl/time/time.cc \
    $$PWD/abseil-cpp/absl/types/bad_any_cast.cc \
    $$PWD/abseil-cpp/absl/types/bad_optional_access.cc \
#    $$PWD/abseil-cpp/absl/types/variant_benchmark.cc \
    $$PWD/abseil-cpp/absl/types/bad_variant_access.cc \
#    $$PWD/abseil-cpp/absl/numeric/bits_benchmark.cc \
    $$PWD/abseil-cpp/absl/numeric/int128.cc \
#    $$PWD/abseil-cpp/absl/numeric/int128_benchmark.cc \
#    $$PWD/abseil-cpp/absl/log/*.cc \
#    $$PWD/abseil-cpp/absl/log/internal/*.cc \
#    $$PWD/abseil-cpp/absl/container/btree_benchmark.cc \
#    $$PWD/abseil-cpp/absl/container/fixed_array_benchmark.cc \
#    $$PWD/abseil-cpp/absl/container/inlined_vector_benchmark.cc \
    $$PWD/abseil-cpp/absl/container/internal/hashtablez_sampler.cc  \
    $$PWD/abseil-cpp/absl/container/internal/hashtablez_sampler_force_weak_definition.cc  \
#    $$PWD/abseil-cpp/absl/container/internal/layout_benchmark.cc  \
    $$PWD/abseil-cpp/absl/container/internal/raw_hash_set.cc  \
#    $$PWD/abseil-cpp/absl/container/internal/raw_hash_set_benchmark.cc  \
#    $$PWD/abseil-cpp/absl/container/internal/raw_hash_set_probe_benchmark.cc  \
#    $$PWD/abseil-cpp/absl/container/internal/test_instance_tracker.cc
