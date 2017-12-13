#include "sorter_client.h"








int main(int argc, char * argv[]){

  
   
	
	
	if (argc<7){


        printf("NOT ENOUGH PARAMTERS\n");
        exit(0);
	}

	

        if(strcasecmp(argv[1], "-c") ==0 ){

        strcpy(temp_sort, argv[2]);
        }
        else{

            printf("WRONG PARAMETERS\n");
            exit(0);
        }



        if(strcasecmp(argv[3], "-h") ==0 ){

            server = gethostbyname(argv[4]);
		host = (char *) argv[4];
        }else{

            printf("WRONG PARAMETERS\n");
            exit(0);
        }
	

	if(strcasecmp(argv[5], "-p") ==0 ){

		portno = atoi(argv[6]);

	}else{

            printf("WRONG PARAMETERS\n");
            exit(0);
        }

	

	/**we should have everything here**/
	if (argc >= 11){


		if(strcasecmp(argv[7], "-d") ==0 ){

		 strcpy(initial_dir_name, argv[8]);

		}
		else{

		    printf("WRONG PARAMETERS\n");
		    exit(0);
		}

		if(strcasecmp(argv[9], "-o") ==0 ){

			 strcpy(output_dir_name,argv[10]);

		}
		else{

		    printf("WRONG PARAMETERS\n");
		    exit(0);
		}




	}

	/**one of the directory is given**/
	else if (argc>7){



        if(strcasecmp(argv[7], "-d") ==0 ){

        strcpy(initial_dir_name, argv[8]);
        strcpy(output_dir_name, "NONE");
        }



        else if(strcasecmp(argv[7], "-o") ==0 ){

            strcpy(output_dir_name, argv[8]);
            getcwd(initial_dir_name,1048);
        }

        else{

            printf("WRONG PARAMETERS\n");
            exit(0);
        }


	}

	else{

        getcwd(initial_dir_name,1048);
        strcpy(output_dir_name, "NONE");

	}


	/**chekcing if the output dir exists**/
	DIR* check = opendir(output_dir_name);
	if (!check && strcmp(output_dir_name,"NONE")!=0){

		printf("WRONG PARMETER. OUTPUT FILE IS NOT CORRECT\n");
		exit(0);

	}

	//printf("sort_by: %s\nserver: %d\n port: %d\ninput dir: %s\noutput dir: %s\n\n\n",temp_sort,gethostbyname(argv[4]),portno, initial_dir_name,output_dir_name);

	

/**-----------------------------------Parameter checking DONE ----------------------------------------------**/


	

/*-----------------------------------ESTABLISHING A CONNECTION WITH THE CLIENT-------------------------------*/



	struct addrinfo hints, *servinfo, *p;
    int rv;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; // use AF_INET6 to force IPv6
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(host, argv[6], &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        exit(1);
    }

    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,p->ai_protocol)) == -1) {
            perror("socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            perror("connect");
            close(sockfd);
            continue;
        }

        break; 
    }
    if (p == NULL) {
        fprintf(stderr, "failed to connect\n");
        exit(2);
    } else{

	printf("connection successful\n");

	}                                                                       


	/*

	//making a socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd <0){

		printf("error making a socekt\n");
		exit(0);
	}

	

	if (server == NULL){


		printf("wront IP Address\n");
		exit(0);
	}


	bzero((char *) &serv_add, sizeof(serv_add));
	serv_add.sin_family = AF_INET;
	bcopy (server->h_addr , (char *) &serv_add.sin_addr.s_addr , server-> h_length);
	serv_add.sin_port = htons(portno);

	if (connect (sockfd , (struct sockaddr *) &serv_add, sizeof(serv_add)) < 0){

		printf("connection error\n");
		exit(0);

	}*/

	int my_id = -1;
	read(sockfd, &my_id , 4);
	//printf("my Id from server: %d\n", my_id);

	


/*----------------------------------CONNECTION ESTABLISHED-------------------------------------------------*/


/*----------------------------------SENDING THE FILES TO THE SERVER-----------------------------------------*/
	
	
	
	
	strcpy(header, "color,director_name,num_critic_for_reviews,duration,director_facebook_likes,actor_3_facebook_likes,actor_2_name,actor_1_facebook_likes,gross,genres,actor_1_name,movie_title,num_voted_users,cast_total_facebook_likes,actor_3_name,facenumber_in_poster,plot_keywords,movie_imdb_link,num_user_for_reviews,language,country,content_rating,budget,title_year,actor_2_facebook_likes,imdb_score,aspect_ratio,movie_facebook_likes,\n");




	

	find_csv_files(initial_dir_name);
/*-----------------------------------------SENT ALL CSV FILES TO THE SERVER----------------------------------------*/

/*----------------------------REQUESTING THE SERVER TO SORT ALL THE DATA-------------------------------------------*/


	char line[1000];

	bzero(line ,1000);
	write(sockfd , "done",4);

	read(sockfd , line , 5);
	bzero(line ,1000);

	bzero(line ,1000);
	write(sockfd, temp_sort, strlen(temp_sort));


/*------------------------------REQUESTING THE SERVER TO SEND THE SORTED DATA BACK AND WRITING IT BACK---------------------------------------*/

	write(sockfd,&my_id, 4);
	
	char output_file_name[50];
	get_output_name(output_dir_name,NULL, output_file_name,strcmp("NONE",output_dir_name)==0? 0: 1);

	
	FILE* output_file = fopen(output_file_name, "w");
	
	if(output_file == NULL){


		printf("could not open the output file: %s\n", output_file_name);
		exit(0);
	}

	fwrite(header , 1 , sizeof(header) , output_file);

	
	bzero(line,1000);

	while ((read(sockfd,line,1000)) > 0){


		if (strncmp("done", line, 4) ==0){
			
			bzero(line , 1000);
			break;

		}
		
		fwrite(line, 1 , sizeof(line), output_file);
		
		bzero(line , 1000);
		write(sockfd, "ok",2);

	}
	fclose(output_file);	
	close(sockfd);

	printf("connection closed successfully\n");

	
	return 0;
	
	
}


void get_output_name(char directory[], char name[],char output[],int check ){

   output[0]= '\0';
    int index=0;
    int lenght = 0;

    if (check!=0){

        strcpy(output,directory);
        strcat(output, "/");

       // printf("String is: %s\n", output);

        index = strlen(output);
    }

    /**
        ->we first get the name before the
        ->file.csv will produce file
    **/


        /*int i;
        for (i=0; i<lenght; i++){

            output[index] = name[i];
            index++;
        }*/

        output[index]= '\0';
        strcat(output, "AllFiles-sorted-");
        index+=16;
        output[index]= '\0';



        /**we have the file name without extention
        ->now we add the temp_sort + .csv
        **/
        lenght = strlen(temp_sort);

         int i;

        for (i=0; i<lenght; i++){

           output[index] = temp_sort[i];
           index++;
        }
        /**now we have the correct format**/
        output[index] = '\0';
        strcat(output,".csv");
	//printf("file name is: %s\n", output);

}



void* thread_dir_func(void * ptr){


    struct names * na  = (struct names *) ptr;
   
   // printf("file is: %s\n dir is: %s\n sort by is: %s\n",na->file, na->dir, na->sort);

    char input_file[100];
    strcpy(input_file,na->file);

    //printf("%d,", pthread_self());   

    find_csv_files(input_file);

    pthread_exit(NULL);
}


int check_csv_format(char *name, char path[]){

    int name_length = strlen(name);

    /**it CANNOT have .csv file**/
    if (name_length < 4){

        return -1;

    }
    char format[5];
    format[0] = '\0';
    int i= 0 ;


    int start_index = name_length -4;

    while (start_index<name_length){

        format[i]  = name[start_index];

        start_index++;
        i++;
    }
    format[i] = '\0';

    if (strcasecmp(format, ".csv")!=0){

         //printf("NON_CSV_FILE: %s\nPath is: %s\n\n",name, path);
        return -1;
    }
   
   return 0;
}



int find_csv_files(char *directory_name){

	
	int limit_2 = 1024;
	struct names * sts_2 = malloc(sizeof(struct names) * limit_2);
	pthread_t* ts_2 = (pthread_t *)malloc(sizeof(pthread_t)*limit_2);
	int ts_index_2= 0;
	

	
        /**variable that will serve for a path name**/
        char path[1048];


       /**making a DIR variable and opening the Directory**/
       DIR *director;
       director = opendir(directory_name);

       /**making direct and stat for name checking**/
	   struct dirent *each_dir;
	   struct stat info;


	   /**error checking for the initial directory**/
	   if (!director){ return -1;}



	/**runs a loop until no files left in the dir**/
	while ((each_dir = readdir(director))!=NULL){


        /**checking if it is its own dir. (dot) .**/
        if (each_dir->d_name[0]== '.'){ /** do nothing**/}


        /**DIRorFILE is not its own file. (dot) .**/
        else{

            /** getting a path name**/
            dir_name(directory_name,each_dir->d_name, path,0);


            /** setting a info stat**/
            stat(path, &info);

            /**if its a sub dir we fork it**/
                
            if (S_ISDIR(info.st_mode)){

              
                   
			
			
			/**reallocating the arraylist**/
			if(ts_index_2 == limit_2){
				
				//pthread_mutex_lock(&lock);
				//printf("allocated in dir\n");				
				limit_2  = limit_2 *3;
				sts_2 = realloc (sts_2, sizeof(struct names) * limit_2);
				ts_2  = realloc (ts_2, sizeof(pthread_t) * limit_2);
				//pthread_mutex_unlock(&lock);	
				
				
			}

			//pthread_mutex_lock(&lock);
			 struct names st;

           		strcpy(st.file, path);
			sts_2[ts_index_2] = st;

			pthread_create(&ts_2[ts_index_2],NULL,thread_dir_func,&sts_2[ts_index_2]);
			
			ts_index_2 +=1;
			
			//pthread_mutex_unlock(&lock);


               
		

            }
		else{


                //check if the file is .csv file by calling the functin check_csv_format() and check if the contens inside the file is corrent : meaning : MOVIE,DIRECTOR_NAME, ....etc

                if (check_csv_format(each_dir->d_name,path)==0){

                  
			
                        
			
			/**reallocating the arraylist**/
			if(ts_index_2 == limit_2){
				//pthread_mutex_lock(&lock);
				//printf("allocated for file\n");
				limit_2  = limit_2 *3;
				sts_2 = realloc (sts_2, sizeof(struct names) * limit_2);
				ts_2  = realloc (ts_2, sizeof(pthread_t) * limit_2);
				//pthread_mutex_unlock(&lock);
				//printf("reallocated to : %d\n", ts_limit);
				
			}

			//pthread_mutex_lock(&lock);
			//printf("lock is acquired");
			struct names st;
           		strcpy(st.file, path);
			sts_2[ts_index_2] = st;

			pthread_create(&ts_2[ts_index_2],NULL,thread_func,&sts_2[ts_index_2]);
			
			ts_index_2 +=1;
			//ts_index++;
			//pthread_mutex_unlock(&lock);


			
                       
			

                    
			
                	}
                
            	}


	}

}
       		int i;
		for (i=0; i<ts_index_2 ; i++){

			
			pthread_join(ts_2[i],NULL);

		}
		free(ts_2);
		free(sts_2);
		return 1;


	}







void dir_name(char* directory, char* name, char path[], int check){

	
    path[0] = '\0';
    strcat(path, directory);
    strcat(path, "/");
	
	if (check==0){
		
		 strcat(path, name);
	}
	else{
		/**we need to come back here**/
		strcat(name, "-");
	}
   
}




void* thread_func(void * ptr){


    struct names * na  = (struct names *) ptr;
   
   // printf("file is: %s\n dir is: %s\n sort by is: %s\n",na->file, na->dir, na->sort);

    char input_file[100];
    strcpy(input_file,na->file);
    //strcpy(output_file,na->dir);

    //printf("%d,", pthread_self());   
   


    sort_file(input_file,NULL);

    pthread_exit(NULL);
}





int sort_file(char input_file_name[],char output_file_name[]){


	/**aquiring the lock**/
	pthread_mutex_lock(&lock);
	
	
	FILE * file = fopen(input_file_name, "r");
	if (file == NULL){


		printf("erorr opening the file:\n%s", input_file_name);
		exit(0);
	}

	char line[1000];

	fgets(line,1000,file);//getting the header

	fgets(line,1000,file); //getting the first line



	 while (strlen(line)>1){

		//printf("%s\n" , line);
		write(sockfd , line,strlen(line));
		
		read(sockfd , line , 5);

		bzero(line ,1000);
		fgets(line,1000,file);

	}


		bzero(line ,1000);
		fclose(file);

		pthread_mutex_unlock(&lock);
		return 0;


		
	
	
}
