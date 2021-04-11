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

		bool input_redir = false;
		char*  input_redir_file;
		char*  output_redir_file;
		bool output_redir = false;
		bool pipe_flag = false;
		int pipe_counter = 0;


			
		int start[20] = {0}; // the start of each process 
 
		for (int k = 0; k < m; k++) // m size of argv
		{
			if (strcmp (argv[k], "<") == 0)
			{
				argv[k] = 0;
				input_redir_file = argv[k + 1];//strdup(argv[k + 1]);
				input_redir = true;
			}
			else if (strcmp (argv[k], ">") == 0) 
			{
				argv[k] = 0;
				output_redir_file = argv[k + 1];
				output_redir = true;
			}
			else if (strcmp (argv[k], "|") == 0)
			{
				pipe_flag = true;
				argv[k] = 0;
				start[pipe_counter + 1] = k + 1; // save position after pipe
				pipe_counter++;
			}

		}

		int l_fd[2]; // file description table
		int r_fd[2];
		//int pid;
		// need to create the child processes & pipes
		for (int k = 0; k <= pipe_counter; k++)
		{
			if (pipe_counter > 0 && k != pipe_counter)//if (/*there are pipes */ && /*not last child*/)
			{	
				printf("creating a pipe\n");
				pipe(r_fd);
			}
			int pid = fork();

			if (pid < 0 ) // error
			{
        		printf("error creating process\n");
			}
			if  (pid > 0) // parent 
			{
				if (k > 0)
				{
					close(l_fd[0]);
					close(l_fd[1]);
				}
				l_fd[0] = r_fd[0]; // saving right pipe to left 
				l_fd[1] = r_fd[1]; // left pipe obtained by assigment
				wait(&status);
			}

			else // child 
			{
				if((input_redir == true) && (k == 0)) // deal with input rederection
				{	
					printf("entered >\n");
					int file_hadler = open(input_redir_file,O_RDONLY , 0400);//,0777);
					close(0); // set standard input invalid
					dup(file_hadler);
					close(file_hadler);// set invalid return to the system
					//input_redir_file = false;
				}
				if (pipe_counter > 0)
				{
					if (k == 0)
					{
						//printf("Handaling first child\n");
						close(1);
						dup(r_fd[1]);
						close(r_fd[1]);
						close(r_fd[0]);
					}
					else if (k < pipe_counter)
					{
						//printf("Handaling middle child\n");
						close(0);
						dup(l_fd[0]);
						close(l_fd[1]);
						close(l_fd[0]);
						close(1);
						close(r_fd[0]);
                        close(r_fd[1]);
					}
					else
					{
						//printf("handaling last child\n");
						close(0);
						dup(l_fd[0]);
						close(r_fd[0]); 
						close(r_fd[1]); 
					}
				}
				if (output_redir && k == pipe_counter)/*output redir && last process*/
				{   // deal with output redirection
					int n = creat(output_redir_file,O_WRONLY);//|O_CREAT,0777);
					close(1);
					dup(n);
					close(n);
				}
				execvp(argv[start[k]], &argv[start[k]]);
			}
		} 
		wait(&status);
	}
}

