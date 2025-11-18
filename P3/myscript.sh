# ===== SIMPLE COMMANDS =====
echo hello world

# ===== INPUT REDIRECTION =====
cat < input.txt

# ===== OUTPUT REDIRECTION =====
echo hi > out1.txt

# View results:
cat out1.txt

# ===== BOTH < AND > =====
sort a b < unsorted.txt > sorted.txt
cat sorted.txt

# ===== BUILT-INS WITHOUT REDIRECTION =====
pwd
cd ..
which ls

# ===== BUILT-INS WITH REDIRECTION =====
pwd > pwdout.txt
cat pwdout.txt

which sort > whichsort.txt
cat whichsort.txt

# ===== PIPELINES =====
echo hello | tr a-z A-Z
cat input.txt | sort | uniq

# ===== PIPELINES WITH BUILT-INS =====
pwd | cat
which ls | cat

# ===== CONDITIONALS: and/or =====
nosuchcmd
echo this should run next

nosuchcmd or echo recovered

echo hi and echo success
echo hi and nosuchcmd
nosuchcmd or echo fallback

# ===== CONDITIONALLY BLOCKED FIRST COMMAND =====
and echo not allowed
or echo not allowed

# ===== INVALID CONDITIONAL INSIDE PIPELINE =====
echo hi | and echo bad
echo hi | or echo bad

# ===== EXIT TEST (Put LAST!) =====
exit

# This should never run:
echo should_not_run