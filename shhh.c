#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <unistd.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(void)
{
	char *path, *argv[20], buf[80], n, *p;
	int m, status, inword, continu;
	
	while(1)
	{
		inword = 0;
		p = buf;
		m = 0;
		continu=0;

		printf( "\nshhh> ");

		while ( ( n = getchar() ) != '\n'  || continu )
		{
			if ( n ==  ' ' )
			{ 
	    		if ( inword )
				{
					inword = 0;
					*p++ = 0;
	    		} 

			}
			else if ( n == '\n' ) continu = 0;

			else if ( n == '\\' && !inword ) continu = 1;

			else
			{
	    		if ( !inword )
				{
		 			inword = 1;
		 			argv[m++] = p;
					*p++ = n;
				}
				else
					*p++ = n;
			}
		}	
		*p++ = 0;
		argv[m] = 0;
		if ( strcmp(argv[0],"exit") == 0 ) exit (0);

		/* Your change starts from here */
		//1.preprocessing the array 

 		int argv_size = sizeof(argv) / sizeof(argv[0]);
		bool input_redir = false;
		char*  input_redir_file;
		char*  output_redir_file;
		bool output_redir = false;
		bool pipe_flag = false;
		int pipe_counter = 0;

			// the start of each process 
		int start[20] = {0};
		printf("%d\n",inword);
		for (int k = 0; k < 1; k++)
		{
			if (strcmp (argv[k], "<") == 0)
			{
				argv[k] = 0;
				input_redir_file = argv[k + 1];
				input_redir = true;
			}
/*			if (strcmp (argv[k], ">") == 0) 
			{
				argv[k] = 0;
				output_redir_file = argv[k + 1];
				printf("\n");
				//printf(output_redir_file);
				printf("\n");
				output_redir = true;
			}
			if (strcmp (argv[k], "|") == 0)
			{
				pipe_flag = true;
				argv[k] = 0;
				pipe_counter++;
				start[pipe_counter] = k + 1; // save position after pipe
			}*/
		}

		int l_fd[2]; // file description table
		int r_fd[2];
		int pid;
		// need to create the child processes & pipes
		for (int k = 0; k <= pipe_counter; k++)
		{
			if (pipe_flag && k != pipe_counter)//if (/*there are pipes */ && /*not last child*/)
			{
				pipe(r_fd);
			}
			pid = fork();
			if (pid < 0 ) // error
			{
        		printf("error creating process\n");
			}
			else if (pid > 0) // parent 
			{
				if (k > 0)
				{
					close(l_fd[0]);
					close(l_fd[1]);
				}
				l_fd[0] = r_fd[0]; // saving right pipe to left 
				l_fd[1] = r_fd[1]; // left pipe obtained by assigment

				//close(1); // ?
			}
			else // child 
			{
				if(input_redir_file && k == 0) // deal with input rederection
				{	
					int file_hadler = open(input_redir_file,777);
					close(0); // set standard input invalid
					dup(file_hadler);
					close(file_hadler);// set invalid return to the system
					input_redir_file = false;
				}

				if (/*there are pipes*/true)
				{
					//3 types of connections 
					close(0);
					dup(r_fd[0]);
					//close((r_fd[0]); ? no because next process will use 
					//close(r_fd[1]); ?
				}
				if (output_redir_file && k == pipe_counter)/*output redir && last process*/
				{   // deal with output redirection
					close(1); // close standard output
					int file_handeler = creat(output_redir_file,O_WRONLY); // create file if does not exit
					dup(file_handeler);
					close(file_handeler);
				}
				

				//execp(argv[?],&argv[?]) // last part 

			}
		} 


		if ( fork() == 0 )
		{
			execvp( argv[0], argv );
			printf ( " didn't exec \n ");
		}
		wait(&status);
	}
}

