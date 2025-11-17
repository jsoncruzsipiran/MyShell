#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/wait.h>

int main(){
    int totalTests = 43;
    int passedTests = 0;
    int failedTests = 0;

    char *testExecutables[] = {
        "./builds/overview", //5
        "./builds/commandFormat", //21
        "./builds/builtInCommands" // 17
    };

    int numTests[] = {5, 21, 17};

    int numSuites = sizeof(testExecutables) / sizeof(testExecutables[0]);
    
    printf("Running %d test suites...\n\n", numSuites);
    
    for (int i = 0; i < numSuites; i++) {
        const char *testPath = testExecutables[i];
        
        printf("========================================\n");
        printf("Running: %s\n", testPath);
        printf("========================================\n");
        
        int status = system(testPath);

        if (status == -1) {
            fprintf(stderr, "Failed to run test suite: %s (system() error)\n", testPath);
            failedTests += numTests[i];
        } else if (WIFEXITED(status)) {
            int failures = WEXITSTATUS(status);
            if (failures < 0) failures = numTests[i];
            if (failures > numTests[i]) failures = numTests[i];
            failedTests += failures;
            passedTests += (numTests[i] - failures);
        } else if (WIFSIGNALED(status)) {
            /* terminated by signal, count entire suite as failed */
            fprintf(stderr, "Test suite %s terminated by signal %d\n", testPath, WTERMSIG(status));
            failedTests += numTests[i];
        } else {
            /* unknown status: treat as full failure */
            failedTests += numTests[i];
        }
    }
    
    printf("\n========================================\n");
    printf("Total Tests Summary:\n");
    printf("  Passed: %d/%d\n", passedTests, totalTests);
    printf("========================================\n");
    
    return (failedTests == 0) ? 0 : 1;
}