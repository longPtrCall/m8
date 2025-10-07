/* * * * * * * * * * * *
 * File: m8.h
 * Description: Guided by chaos, M8 is a lightweight, single-header build system for C and C++.
 *
 * Simple example:
 * ```C
 * #include <m8.h>
 *
 *
 * int main(const int argc, const char* const argv[]) {
 *
 *   compiler_arguments = "-O2 -pipe -fPIC -march=native -flto=thin -Wall -Wextra";
 *   const char* const source_files[] = { "main.cxx", "test.cxx" };
 *   compiler = "clang++ -c", linker = "clang++";
 *   output = "test"_executable;
 *   objects = "oxx";
 *
 *   return m8_main(argc, argv, enumerate(source_files), enumerate(default_build_commands));
 * }
 * ```
 *
 * Copyright 2025 Vitaliy Konko
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * * */
#ifndef __m8_h__
#define __m8_h__

#include <stdbool.h>
#include <assert.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef _WIN32
  #include <windows.h>
  #include <winbase.h>

  #define thread_return_t DWORD WINAPI
  typedef LPVOID thread_arg_t;
  typedef HANDLE thread_t;

  #define __m8cc "cl /Fe:%s m8.c"
  #define _executable ".exe"
  #define _shared ".dll"
  #define _static ".lib"

  #define __path_delim "\\"
  #define _endl "\r\n"

  #define mkdir(path, __) (CreateDirectory(path, NULL) == TRUE ? 0 : -1)
#else
  #include <sys/stat.h>
  #include <pthread.h>

  #define thread_return_t void*
  typedef void* thread_arg_t;
  typedef pthread_t thread_t;

  #define __m8cc "cc -lpthread -o %s m8.c"
  #define _executable ""
  #define _shared ".so"
  #define _static ".a"

  #define __path_delim "/"
  #ifdef __APPLE__
    #define _endl "\r"
  #else
    #define _endl "\n"
  #endif
#endif

#ifndef m8cc
  #define m8cc __m8cc
#endif

#ifndef countof
  #define countof(array) (sizeof array / sizeof *array)
#endif
#define enumerate(array) countof(array), array

static char* source_dir = "src", *build_dir = "build", *dist_dir = "dist";
static char* compiler = "cc -c", *compiler_arguments = "-O2";
static char* linker = "ld", *linker_arguments = "";
static char* output = "output"_executable;
static char* install_prefix = "/usr";
static char* objects = "o";
static char* ar = "ar";

// If this project is a libray, user may want to export headers as well.
static size_t header_files_count = 0;
static char** header_files = NULL;

typedef enum __project_type_t {
  PROJECT_TYPE_EXECUTABLE,
  PROJECT_TYPE_STATIC_LIBRARY,
  PROJECT_TYPE_SHARED_LIBRARY,
} project_type_t;

static project_type_t project_type = PROJECT_TYPE_EXECUTABLE;


typedef int(*build_command_function_t)(const int, const char* const[], const int, const char* const[]);

typedef struct __build_command_t {
  const char* const name, * const description;
  build_command_function_t function;
} build_command_t;


/* * *
 * M8 main function. Performs all logic, including self-regeneration and building.
 *
 * Arguments:
 * - argc - command line arguments count.
 * - argv - command line arguments.
 * - srcc - source files count.
 * - srcv - source files.
 * - cmdc - build commands count.
 * - cmdv - build commands.
 * Returns subcommand exit status.
 */
static int m8_main(
  const int argc,
  const char* const argv[],
  const int srcc,
  const char* const srcv[],
  const int cmdc,
  const build_command_t cmdv[]
);


/* * *
 * User help function.
 *
 * Arguments:
 * - me   - the name of this executable (m8).
 * - cmdc - build commands count.
 * - cmdv - build commands.
 */
static void m8_help(const char* const me, const int cmdc, const build_command_t cmdv[]);


/* * *
 * Default build function. Compiles all sources into objects and links target binary.
 *
 * Arguments:
 * - argc - command line arguments count.
 * - argv - command line arguments.
 * - srcc - source files count.
 * - srcv - source files.
 * Returns zero on success.
 */
static int m8_build(const int argc, const char* const argv[], const int srcc, const char* const srcv[]);


/* * *
 * Default install function. Copies dist tree to a specified directory.
 *
 * Arguments:
 * - argc - command line arguments count.
 * - argv - command line arguments.
 * - srcc - source files count.
 * - srcv - source files.
 * Returns zero on success.
 */
static int m8_install(const int argc, const char* const argv[], const int srcc, const char* const srcv[]);


/* * *
 * Default uninstall function. Removed all files copied by `install`.
 *
 * Arguments:
 * - argc - command line arguments count.
 * - argv - command line arguments.
 * - srcc - source files count.
 * - srcv - source files.
 * Returns zero on success.
 */
static int m8_uninstall(const int argc, const char* const argv[], const int srcc, const char* const srcv[]);


/* * *
 * Default clean function. Removes all files from `dist` and `build`.
 *
 * Arguments:
 * - argc - command line arguments count.
 * - argv - command line arguments.
 * - srcc - source files count.
 * - srcv - source files.
 * Returns zero on success.
 */
static int m8_clean(const int argc, const char* const argv[], const int srcc, const char* const srcv[]);


typedef struct __m8_compilation_list_t {
  size_t count;
  char** srcv;
  char** objv;
} m8_compilation_list_t;


/* * *
 * Convert a list of source files into target files.
 *
 * Arguments:
 * - data - instance of `m8_compilation_list_t`;
 * Returns zero on success (compiler exit code).
 */
static thread_return_t m8_compile(const thread_arg_t data);


/* * *
 * Perform object linkage.
 *
 * Arguments:
 * - objc - object files count.
 * - objv - object files.
 * Returns linkage status.
 */
static int m8_link(const int objc, const char* const objv[]);


/* * *
 * Construct a list of object files from source files.
 *
 * Arguments:
 * - srcc - source files paths count.
 * - srcv - source files paths.
 * Returns a list of object files paths.
 */
static char** __get_object_files(const int srcc, const char* const srcv[]);


/* * *
 * Free the memory, allocated for object files list.
 *
 * Arguments:
 * - count - object files count.
 * - files - a pointer to the array.
 */
static inline void __free_object_files(const int count, char** const files);


/* * *
 * Create a filesystem tree for the build. Currently supports the only layout:
 * ```
 * .
 * |- build
 * |- dist
 * |  |-include
 * |  |-bin
 * |  |-lib
 * ```
 * TODO: More layouts will be supported in future.
 */
static void __setup_tree(void);


/* * *
 * Get the number of available jobs. Default return value is 1.
 *
 * Arguments:
 * - argc - command line arguments count.
 * - argv - command line arguments.
 * Returns a number of jobs parsed from `-j N`.
 */
static int __get_jobs(const int argc, const char* const argv[]);


/* * *
 * Create and run a thread.
 *
 * Arguments:
 * - function - function to execute.
 * - argument - argument to pass to a `function`.
 * Returns created thread.
 */
thread_t __create_thread(thread_return_t(*function)(thread_arg_t), thread_arg_t argument);


/* * *
 * Wait for threads.
 *
 * Arguments:
 * - count   - threads count.
 * - threads - array of threads to wait for.
 */
void __wait_jobs(size_t count, thread_t* threads);


/* * *
 * Get target binary path.
 * Returns a project-relative target path.
 */
static char* __get_target_path(void);


/* * *
 * Copy one file to another location.
 *
 * Arguments:
 * - source      - file to copy.
 * - destanation - target path.
 * Returns zero on success.
 */
static inline int __copy(const char* const source, const char* const destanation);


static build_command_t default_build_commands[] = {
  {
    .name = "build",
    .description = "Compile and link source files."
                   "Add `j N` or `--jobs N` options, where N is a number of threads to utilize.",
    .function = &m8_build
  },
#ifndef _WIN32
  {
    .name = "install",
    .description = "Copy a dist tree to a specified directory.",
    .function = &m8_install
  },
  {
    .name = "uninstall",
    .description = "Remove all installed files.",
    .function = &m8_uninstall
  },
#endif
  {
    .name = "clean",
    .description = "Remove all temporary build files and dist tree.",
    .function = &m8_clean
  },
};


static int m8_main(
  const int argc,
  const char* const argv[],
  const int srcc,
  const char* const srcv[],
  const int cmdc,
  const build_command_t cmdv[]
) {

  assert(cmdc && cmdv && "Build commands must be specified. To use default type `enumerate(default_build_commands)`");
  assert(srcc && srcv && "Source files must be specified.");

  // TODO: Regenerate.

  const char* const command = (argc > 1) ? argv[1] : (const char*)0;
  const char* const me = *argv;
  if (command && strcmp("help", command) == 0) m8_help(me, cmdc, cmdv);
  else {

    if (command) {

      for (const build_command_t* current = cmdv; current < cmdv + cmdc; current++)
        if (strcmp(current->name, command) == 0)
          return current->function(argc, argv, srcc, srcv);

      printf("[E] Command not found: `%s`. Run `%s help` to list available commands." _endl, command, me);
      return 127;
    } else return cmdv->function(argc, argv, srcc, srcv);
  }
  return 0;
}


static void m8_help(const char* const me, const int cmdc, const build_command_t cmdv[]) {

  printf("This is your m8. I'm here to build software for you!" _endl);
  printf("Usage: %s [command] <options>" _endl, me);
  printf("Defaults to the first available command if no command is specified. Available commands:" _endl _endl);
  for (const build_command_t* command = cmdv; command < cmdv + cmdc; command++)
    printf(" - %s - %s" _endl _endl, command->name, command->description);
  return;
}


static int m8_build(const int argc, const char* const argv[], const int srcc, const char* const srcv[]) {

  printf("= = = [COMPILING] = = = = = = = = = = = =" _endl);
  const int jobs = __get_jobs(argc, argv);
  const int threads_count = jobs < srcc ? jobs : srcc;
  const size_t job_sources = srcc / threads_count, remaining_sources = srcc % threads_count;
  printf("[I] Using %d jobs" _endl, threads_count);

  __setup_tree();
  char** object_files = __get_object_files(srcc, srcv);

  m8_compilation_list_t* lists = (m8_compilation_list_t*)calloc(threads_count, sizeof(m8_compilation_list_t));
  thread_t* threads = (thread_t*)calloc(threads_count, sizeof (thread_t));
  for (size_t thread_id = 0; thread_id < threads_count; thread_id++) {

  	lists[thread_id].count = job_sources;
	lists[thread_id].srcv = (char**)srcv + job_sources * thread_id;
    lists[thread_id].objv = object_files + job_sources * thread_id;
    threads[thread_id] = __create_thread(&m8_compile, &lists[thread_id]);
  }
  __wait_jobs(threads_count, threads);
  free(threads);
  free(lists);

  if (remaining_sources) {

    m8_compilation_list_t remained_list = {
      .count = remaining_sources,
      .srcv = (char**)srcv + srcc - remaining_sources,
      .objv = object_files + srcc - remaining_sources
    };
    m8_compile(&remained_list);
  }
  printf("- - - [LINKING] - - - - - - - - - - - - -" _endl);
  m8_link(srcc, (const char* const*)object_files);
  __free_object_files(srcc, object_files);

  if (header_files_count) {

    printf("- - - [HEADERS] - - - - - - - - - - - - -" _endl);
    for (size_t header_file_id = 0; header_file_id < header_files_count; header_file_id++) {

      char* const file_name = header_files[header_file_id];
      char path[256] = { 0 }, copy_path[256] = { 0 };
      sprintf(path, "%s" __path_delim "%s", source_dir, file_name);
      sprintf(copy_path, "%s" __path_delim "include" __path_delim "%s", dist_dir, file_name);
      printf("[I] Copy %s -> %s (%ld/%ld)... %s" _endl, path, copy_path, header_file_id + 1, header_files_count, __copy(path, copy_path) == 0 ? "[OK]" : "[FAILED]");
    }
  }
  printf("[I] Compiled successfully." _endl);
  return 0;
}


static int m8_install(const int argc, const char* const argv[], const int srcc, const char* const srcv[]) {

  // TODO: Add support for windows.
  char install_target[512] = { 0 };
  printf("= = = [INSTALL] = = = = = = = = = = = =" _endl);
  sprintf(install_target, "%s/%s/%s", install_prefix, (project_type == PROJECT_TYPE_EXECUTABLE ? "bin" : "lib"), output);
  printf("[I] Copy %s -> %s:\t\t\t%s" _endl, __get_target_path(), install_target, __copy(__get_target_path(), install_target) == 0 ? "[OK]" : "[FAILED]");
  if (header_files_count) {

   for (size_t header_file_id = 0; header_file_id < header_files_count; header_file_id++) {

      char* const file_name = header_files[header_file_id];
      char path[256] = { 0 }, copy_path[256] = { 0 };
      sprintf(path, "%s" __path_delim "include" __path_delim "%s", dist_dir, file_name);
      sprintf(copy_path, "%s%s" __path_delim "%s", install_prefix, "/include", file_name);
      printf("[I] Copy %s -> %s (%ld/%ld)... %s" _endl, path, copy_path, header_file_id + 1, header_files_count, __copy(path, copy_path) == 0 ? "[OK]" : "[FAILED]");
    }
  }
  printf("[I] Installation complete." _endl);
  return 0;
}


static int m8_uninstall(const int argc, const char* const argv[], const int srcc, const char* const srcv[]) {

  // TODO: Add support for windows.
  char install_target[512] = { 0 };
  printf("= = = [UNINSTALL] = = = = = = = = = = = =" _endl);
  sprintf(install_target, "%s/%s/%s", install_prefix, (project_type == PROJECT_TYPE_EXECUTABLE ? "bin" : "lib"), output);
  printf("[I] Remove %s:\t\t\t%s" _endl, install_target, remove(install_target) == 0 ? "[OK]" : "[FAILED]");
  if (header_files_count) {

   for (size_t header_file_id = 0; header_file_id < header_files_count; header_file_id++) {

      char* const file_name = header_files[header_file_id];
      char copy_path[256] = { 0 };
      sprintf(copy_path, "%s" __path_delim "%s", "/usr/include", file_name);
      printf("[I] Remove %s (%ld/%ld)... %s" _endl, copy_path, header_file_id + 1, header_files_count, remove(copy_path) == 0 ? "[OK]" : "[FAILED]");
    }
  }
  printf("[I] Uninstalled." _endl);
  return 0;
}


static int m8_clean(const int argc, const char* const argv[], const int srcc, const char* const srcv[]) {

  printf("= = = [CLEAN] = = = = = = = = = = = =" _endl);
  char** object_files = __get_object_files(srcc, srcv);
  for (size_t object_id = 0; object_id < srcc; object_id++) {

    printf("[I] Removing %s...\t\t\t", object_files[object_id]);
    printf("%s" _endl, remove(object_files[object_id]) == 0 ? "[OK]" : "[FAILED]");
  }
  printf("[I] Removing target...\t\t\t%s" _endl, remove(__get_target_path()) == 0 ? "[OK]" : "[FAILED]");
  __free_object_files(srcc, object_files);
  return 0;
}


static thread_return_t m8_compile(const thread_arg_t data) {

  const m8_compilation_list_t* const list = (const m8_compilation_list_t*)data;
  // TODO: Compute the command size.
  char* command = (char*)malloc(8192);
  for (size_t index = 0; index < list->count; index++) {

    // TODO: Add formatting options for Windows.
    const char* format = "%s %s -o %s %s%s%s";
    sprintf(command, format, compiler, compiler_arguments, list->objv[index], source_dir, __path_delim, list->srcv[index]);
    printf("[I] Executing (%ld/%ld): %s" _endl, index + 1, list->count, command);
    const int status = system(command);
    if (status) {

      printf("[E] Compiler returned non-zero value: %d. Aborting." _endl, status);
      free(command);
      exit(status);
    }
  }
  free(command);
  return 0;
}


static int m8_link(const int objc, const char* const objv[]) {

  // TODO: Compute the command size.
  char* command = (char*)malloc(512 + objc * 256);
  // TODO: Support Windows.
  if (project_type == PROJECT_TYPE_STATIC_LIBRARY) {
    sprintf(command, "%s r -o %s", ar, __get_target_path());
  } else sprintf(command, "%s -o %s", linker, __get_target_path());
  for (size_t index = 0; index < objc; index++) {

    strcat(command, " ");
    strcat(command, objv[index]);
  }
  strcat(command, " ");
  strcat(command, linker_arguments);
  printf("[I] Executing: %s" _endl, command);
  const int status = system(command);
  free(command);
  return status;
}


static char** __get_object_files(const int srcc, const char* const srcv[]) {

  const size_t object_prefix_length = strlen(build_dir) + strlen(objects) + 2;
  char** object_files = (char**)calloc(srcc, sizeof *srcv);
  for (size_t index = 0; index < srcc; index++) {

    const size_t source_length = strlen(srcv[index]), object_length = source_length + object_prefix_length;
    object_files[index] = (char*)calloc(object_length, 1);
    sprintf(object_files[index], "%s%s%s.%s", build_dir, __path_delim, srcv[index], objects);
    for (char* symbol = object_files[index] + object_prefix_length + 1; symbol < object_files[index] + object_length; symbol++)
      if (*symbol == __path_delim[0]) *symbol = '.';
  }
  return object_files;
}


static inline void __free_object_files(const int count, char** const files) {

  for (size_t index = 0; index < count; index++)
    free(files[index]);
  free(files);
  return;
}


static void __setup_tree(void) {

  mkdir(build_dir, 0755);
  mkdir(dist_dir, 0755);
  char buffer[260] = { 0 };
  sprintf(buffer, "%s" __path_delim "%s", dist_dir, "include");
  mkdir(buffer, 0755);
  sprintf(buffer, "%s" __path_delim "%s", dist_dir, "bin");
  mkdir(buffer, 0755);
  sprintf(buffer, "%s" __path_delim "%s", dist_dir, "lib");
  mkdir(buffer, 0755);
  return;
}

static int __get_jobs(const int argc, const char* const argv[]) {

  for (size_t index = 0; index < argc - 1; index++) {

    if (strcmp("-j", argv[index]) == 0 || strcmp("--jobs", argv[index]) == 0) {

      const int jobs = atoi(argv[index + 1]);
      return jobs ? jobs : 1;
    }
  }
  return 1;
}


thread_t __create_thread(thread_return_t(*function)(thread_arg_t), thread_arg_t argument) {

  #ifdef _WIN32
    return CreateThread(NULL, 0, function, argument, 0, NULL);
  #else
    pthread_t thread = (pthread_t)0;
    pthread_create(&thread, NULL, function, argument);
    return thread;
  #endif
  assert(false && "Threading implementation is not supported on this host.");
  return 0;
}


void __wait_jobs(size_t count, thread_t* threads) {

  #ifdef _WIN32
    WaitForMultipleObjects(count, threads, TRUE, INFINITE);
  #else
    for (size_t thread_id = 0; thread_id < count; thread_id++)
      pthread_join(threads[thread_id], NULL);
  #endif
  return;
}

static char* __get_target_path(void) {

  static char buffer[256] = { 0 };
  sprintf(buffer, "%s"__path_delim"%s"__path_delim"%s", dist_dir, (project_type == PROJECT_TYPE_EXECUTABLE ? "bin" : "lib"), output);
  return buffer;
}


static inline int __copy(const char* const source, const char* const destanation) {

  char buffer[512] = { 0 };
  #ifdef _WIN32
    static const char* const copy = "copy";
  #else
    static const char* const copy = "cp";
  #endif
  sprintf(buffer, "%s %s %s", copy, source, destanation);
  return system(buffer);
}

#endif
