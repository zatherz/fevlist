#include "fmod_studio.hpp"
#include "fmod.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define EVENT_PATH_MAX_LEN 1024

static void panic_on_err(FMOD_RESULT result) {
    if (result != FMOD_OK) {
        fprintf(stderr, "panic! error code: %d\n", result);
        exit(1);
    }
}

static void sleep(int ms) {
    timespec sleep_time = { 0, ms * 1000 * 1000 };
    nanosleep(&sleep_time, NULL);
}

static void help(FILE* f) {
    fprintf(f, "help: fevlist -h|--help\n");
    fprintf(f, "usage: fevlist [-v|--verbose -d|--fmod-debug] PATH_TO_MASTER_STRINGS_BANK PATH_TO_TARGET_BANK\n");
    fprintf(f, "\n");
    fprintf(f, "       the tool requires you to provide the 'Master Bank.strings.bank' file on top\n");
    fprintf(f, "       of the one you're actually interested in, because that's what contains information\n");
    fprintf(f, "       about event names\n");
}

int main(int argc, const char** argv) {
    if (argc < 3) {
        if (argc == 2 && strcmp(argv[1], "-h") == 0) {
        
        }
        help(stderr);
        return 1;
    }

    int verbose = 0;
    int fmod_debug = 0;
    size_t positional_args_offs = 0;

    for (int i = 1; i < argc; i++) {
        const char* option = argv[i];
        size_t option_len = strlen(option);


        if (option_len >= 1 && option[0] == '-') {
            positional_args_offs += 1;

            if (option_len >= 2) {
                if (strcmp(option, "-v") == 0 || strcmp(option, "--verbose") == 0) {
                    verbose = 1;
                } else if (strcmp(option, "-d") == 0 || strcmp(option, "--fmod-debug") == 0) {
                    fmod_debug = 1;
                } else if (strcmp(option, "-h") == 0 || strcmp(option, "--help") == 0){
                    help(stdout);
                    return 0;
                } else {
                    fprintf(stderr, "unknown option: %s\n", option);
                    help(stderr);
                    return 1;
                }
            } else {
                fprintf(stderr, "invalid option: %s\n", option);
                help(stderr);
                return 1;
            }
        } else {
            break;
        }
    }

#ifdef DEBUG
    if (!fmod_debug) {
        panic_on_err(FMOD::Debug_Initialize(FMOD_DEBUG_LEVEL_NONE, FMOD_DEBUG_MODE_TTY, 0, nullptr));
    }
#endif

    const char* master_strings_bank_path = argv[positional_args_offs + 1];
    const char* target_bank_path = argv[positional_args_offs + 2];

    if (verbose) {
        printf("listing events from bank '%s' based on strings bank '%s'\n", target_bank_path, master_strings_bank_path);
        printf("creating system\n");
    }

    FMOD::Studio::System* system;
    panic_on_err(FMOD::Studio::System::create(&system));
    panic_on_err(system->initialize(1024, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, NULL));

    if (verbose) {
        printf("system created: 0x%zx\n", (size_t)system);
        printf("loading strings bank\n");
    }

    FMOD::Studio::Bank* master_strings_bank;
    panic_on_err(system->loadBankFile(master_strings_bank_path, FMOD_STUDIO_LOAD_BANK_NORMAL, &master_strings_bank));

    if (verbose) {
        printf("strings bank loaded: 0x%zx\n", (size_t)master_strings_bank);
        printf("loading string bank sample data\n");
    }

    panic_on_err(master_strings_bank->loadSampleData());
    panic_on_err(system->flushSampleLoading());

    if (verbose) {
        printf("string bank sample data loaded\n");
        printf("loading target bank\n");
    }

    FMOD::Studio::Bank* target_bank;
    panic_on_err(system->loadBankFile(target_bank_path, FMOD_STUDIO_LOAD_BANK_NORMAL, &target_bank));

    if (verbose) {
        printf("target bank loaded: 0x%zx\n", (size_t)target_bank);
        printf("loading target bank sample data\n");
    }

    panic_on_err(target_bank->loadSampleData());
    panic_on_err(system->flushSampleLoading());

    if (verbose) printf("target bank sample data loaded\n");

    int target_event_count = 0;
    panic_on_err(target_bank->getEventCount(&target_event_count));

    FMOD::Studio::EventDescription** target_event_ary = (FMOD::Studio::EventDescription**)malloc(sizeof(FMOD::Studio::EventDescription*) * target_event_count);

    if (verbose) printf("obtaining event list (%d entries)\n", target_event_count);

    panic_on_err(target_bank->getEventList(target_event_ary, target_event_count, NULL));

    for (int i = 0; i < target_event_count; i++) {
        FMOD::Studio::EventDescription* event = target_event_ary[i];

        char* path = (char*)malloc(sizeof(char) * EVENT_PATH_MAX_LEN);
        FMOD_GUID id;

        panic_on_err(event->getPath(path, EVENT_PATH_MAX_LEN, NULL));
        panic_on_err(event->getID(&id));

        if (verbose) {
            printf("event %d: path = '%s', guid = '%x-%04x-%04x-%01x%01x-%01x%01x%01x%01x%01x%01x'\n", i, path, id.Data1, id.Data2, id.Data3, id.Data4[0], id.Data4[1], id.Data4[2], id.Data4[3], id.Data4[4], id.Data4[5], id.Data4[6], id.Data4[7]);
        } else {
            printf("{%x-%04x-%04x-%01x%01x-%01x%01x%01x%01x%01x%01x} %s\n", id.Data1, id.Data2, id.Data3, id.Data4[0], id.Data4[1], id.Data4[2], id.Data4[3], id.Data4[4], id.Data4[5], id.Data4[6], id.Data4[7], path);
        }

        free(path);
    }

    free(target_event_ary);

    if (verbose) printf("done! unloading\n");

    panic_on_err(system->unloadAll());
    panic_on_err(system->flushCommands());
    panic_on_err(system->release());

    return 0;
}
