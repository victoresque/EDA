1. Name: readme
2. Description: describe PA1.tgz and its contents
3. Contents:
    (1) testcase          | directory
    (2) testcase_solution | directory
    (3) checker           | binary (Linux)
    (4) readme
	
4. Detailed Contents:
    (1) testcase 
        Include 3 testcases(e.g., testcase1.in).
        
    (2) testcase_solution
        Include 3 the Gurobi solution(eg., testcase1_solution.sol) of each testcase.
        Notice: Input in this programming assignment is [.in].
                Output is [.lp]. 
                [.sol] is the file given by Gurobi after it solved the lp file.
        You can check whether the "objective value" of your solution and this solution are the same.
    
    (3) checker
        Before you use it, you may type "chmod 755 checker" in terminal, or you may fail to use it.
        This checker will check whether your solution is "valid", but not the optimality.
        In other words, you need to check the "objective value" in "testcase_solution" by yourself.
        Usage: ./checker [.in] [.sol]
        
        If checker succeeds, expected output will be as follows.
        ------------------------------------
        Via size from input file: 4
        Objective Value of your solution: 4
        ------------------------------------
        Congratulations!! Check success.
        Your solution is valid.
        ------------------------------------
        
        If checker fails, expected output will be as follows.
        ------------------------------------
        Via size from input file: 4
        Objective Value of your solution: 4
        Error: Via R_5_3 has more than one double-cut type: TYPE 2 TYPE 3
        ------------------------------------
        Checking fail.
        Solution has conflict in same coordinate.
        ------------------------------------
