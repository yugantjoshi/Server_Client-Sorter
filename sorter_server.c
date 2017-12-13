/*#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>
#include<ctype.h>
#include<math.h>
#include <sys/types.h>
#include<sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include<pthread.h>

*/
#include "sorter_server.h"
#include "mergesort.c"


//void* thread_func(void *);


int main(int argc, char * argv[]){


	if (argc < 3 || strcmp(argv[1], "-p")!= 0){

		printf("INVALID PARAMETERS\n");
		exit(0);

	}

	

	ts = (pthread_t *) malloc(sizeof (pthread_t) * ts_limit);
	sockets = (struct socket *) malloc (sizeof(struct socket) * ts_limit);

	int sockfd , clientfd, n, portno;
	char buffer[255];

	struct sockaddr_in serv_add , client_add;
	socklen_t clilen;

	//making server socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd <0){

		printf("error making a socekt\n");
		exit(0);
	}
	int opt = 1;

	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                    &opt, sizeof(opt)))
        {
            perror("setsockopt");
                exit(EXIT_FAILURE);
        }

	bzero((char *) &serv_add, sizeof(serv_add));
	portno = atoi(argv[2]);

	serv_add.sin_family = AF_INET;
	serv_add.sin_addr.s_addr = INADDR_ANY;
	serv_add.sin_port = htons(portno);

	if (bind(sockfd , (struct sockaddr *) &serv_add , sizeof(serv_add)) < 0){


		printf("error binding the socket\n");
		exit(0);

	}


	//lisetining
	listen(sockfd, 5);
	//clilen = sizeof(client_add);
	clilens[client_index] = sizeof(clients_addr[client_index]);

	printf("waiting for connection");
	printf("\n");
	printf("Recieved connections from: ");
	fflush(stdout);


	while (1){
		//printf("accepting the conenction from someone\n");
		//accepting
		clients_sock[client_index] = accept(sockfd , (struct sockaddr *) &clients_addr[client_index] , &clilens[client_index]);

		if (clients_sock[client_index] < 0){

			printf("error making the client socket\n");
			exit(0);

		}

		//printf("connection_successful\n");
		//printf("%s:%d join in! ID(fd):%d \n",inet_ntoa(clients_addr[client_index].sin_addr),
		//ntohs(clients_addr[client_index].sin_port),clients_sock[client_index]);
		printf("<%s>, ",inet_ntoa(clients_addr[client_index].sin_addr));
		fflush(stdout);

		//clientfd = clients_sock[client_index];
		//get_client_contents(clients_sock[client_index]);

		struct socket my_socket;
		my_socket.client_socket = clients_sock[client_index];
		my_socket.my_num = client_index;
		


		sockets[client_index] = my_socket;
		 		
		pthread_create(&ts[client_index],NULL,thread_func,&sockets[client_index]);
			
		client_index ++;

	}

	

	/** Joining the theads**/
	int i;

	for (i=0; i< client_index; i++){

		//printf("%d\n", &ts[i]);
		pthread_join(ts[i], NULL);

	}


	close(sockfd);
	printf("server closed\n");
	
	return 0;

	


}

void* thread_func(void * ptr){


    struct socket * na  = (struct socket *) ptr;
   
    char file_name[15];
    sprintf(file_name, "%d", na->my_num);   

    
    int clientfd = na->client_socket;
    get_client_contents(clientfd,file_name);

    pthread_exit(NULL);
}


void get_client_contents(int clientfd, char file_name[]){


	//sending the ID
pthread_mutex_lock(&merge_lock);
	write(clientfd, &ID,4);
	ID++;
pthread_mutex_unlock(&merge_lock);

	

	char line[1000];
	int n = 0; 
	Movie data;
	  
	     
	     char token[500]; token[0]= '\0';


	     Movie* temp_mvs;
	     int temp_target=5000;
	     int temp_total_num_of_movies=0;

	     temp_mvs = (Movie *) malloc(sizeof(Movie)*temp_target);

	      int track_tokenizer=0;

	
	/*FILE* file = fopen(file_index==0? "first.txt":"second.txt" ,"w");
	file_index ++;
	if (file==NULL){

		printf("error opening the file: %s\n" , file);
		exit(0);
	}*/

	

	while ((read(clientfd,line,1000)) > 0){



		if (strncmp("done", line, 4) ==0){
			
			//write(clientfd , "done" , 4);
			bzero(line , 1000);
			break;

		}

		//printf("%s\n" , line);

		

	     
	

	
		if(temp_total_num_of_movies==temp_target){ //need to realloc

			temp_target = temp_target*3;
			temp_mvs = realloc(temp_mvs, sizeof(Movie)*(temp_target));
			//printf("new target %d\n", target);

		}


		    //value of color, putting the value of color into the data
		 track_tokenizer = subTokenize(line, token,track_tokenizer);
		data.color[0] = '\0';
		trim(token);
		strcpy(data.color, token);

		
		      track_tokenizer = subTokenize(line, token,track_tokenizer);
		data.director_name[0] = '\0';
		trim(token);
		strcpy(data.director_name,token);

		     // printf("%s\n", token);




		    //num_critcs_for_reviews
		    track_tokenizer = subTokenize(line, token,track_tokenizer);
		    trim(token);
		    data.num_critic_for_reviews = atoi(token);
		     //printf("%d\n", atoi(token));

		    //duration
		 track_tokenizer = subTokenize(line, token,track_tokenizer);
		trim(token);
		 data.duration = atoi(token);
		 // printf("%d\n", atoi(token));

		  //director_facebook_likes
		track_tokenizer = subTokenize(line, token,track_tokenizer);
		trim(token);
		data.director_facebook_likes = atoi(token);

		// actor_3_facebook_liked
		  track_tokenizer = subTokenize(line, token,track_tokenizer);
		trim(token);
		  data.actor_3_facebook_likes = atoi(token);
		  //printf("%d\n", atoi(token));

		//actor_to_name
		 track_tokenizer = subTokenize(line, token,track_tokenizer);
		 data.actor_2_name[0] = '\0';
		trim(token);
		 strcpy(data.actor_2_name, token);
		// printf("%s\n", token);


		    //actor_1_facebook_likes
		   track_tokenizer = subTokenize(line, token,track_tokenizer);
			trim(token);
		   data.actor_1_facebook_likes = atoi(token);
		  // printf("%d\n", atoi(token));

		   //gross
		   track_tokenizer = subTokenize(line, token,track_tokenizer);
		trim(token);
		   data.gross = atoi(token);
		  // printf("%d\n", atoi(token));


		//genres
		track_tokenizer = subTokenize(line, token,track_tokenizer);
		data.genres[0]= '\0';
		trim(token);
		strcpy(data.genres, token);
		// printf("%s\n", token);



		  //actor_1_name
		  track_tokenizer = subTokenize(line, token,track_tokenizer);
		  data.actor_1_name[0] = '\0';
		trim(token);
		  strcpy(data.actor_1_name, token);
		 // printf("%s\n", token);


		//movie_title
		track_tokenizer = subTokenize(line, token,track_tokenizer);
		data.movie_title[0]= '\0';
		trim(token);
		strcpy(data.movie_title, token);
		//printf("%s\n", data.movie_title);


		 //num_voted_users
		track_tokenizer = subTokenize(line, token,track_tokenizer);
		trim(token);
		data.num_voted_users = atoi(token);

		//cast_total_Facebook_likes
	       track_tokenizer = subTokenize(line, token,track_tokenizer);
		trim(token);
	       data.cast_total_facebook_likes = atoi(token);

		   //actor_3_name
		    track_tokenizer = subTokenize(line, token,track_tokenizer);
		    data.actor_3_name[0]= '\0';
		trim(token);
		    strcpy(data.actor_3_name, token);
		   // printf("%s\n", token);


		  //facenumber_in_poster
		  track_tokenizer = subTokenize(line, token,track_tokenizer);
		  data.facenumber_in_poster = atoi(token);
		trim(token);
		  // printf("%d\n", atoi(token));

		 // plot keywords
		  track_tokenizer = subTokenize(line, token,track_tokenizer);
		  data.plot_keywords[0] = '\0';
		trim(token);
		  strcpy(data.plot_keywords, token);
		 // printf("%s\n", token);


		 // movie_imdb_link
		  track_tokenizer = subTokenize(line, token,track_tokenizer);
		  data.movie_imdb_link[0] = '\0';
		trim(token);
		  strcpy(data.movie_imdb_link, token);
		 // printf("%s\n", token);


		 // num_user_for_reviews
		  track_tokenizer = subTokenize(line, token,track_tokenizer);
		trim(token);
		  data.num_user_for_reviews = atoi(token);
		  //printf("%d\n", atoi(token));

		 // language
		  track_tokenizer = subTokenize(line, token,track_tokenizer);
		  data.language[0]= '\0';
		trim(token);
		   strcpy(data.language, token);
		  //printf("%s\n", token);


		 // country
		  track_tokenizer = subTokenize(line, token,track_tokenizer);
		  data.country[0] = '\0';
		trim(token);
		  strcpy(data.country, token);
		  //printf("%s\n", token);


		 // content_rating
		 track_tokenizer = subTokenize(line, token,track_tokenizer);
		 data.content_rating[0] = '\0';
		trim(token);
		 strcpy(data.content_rating, token);
		 // printf("%s\n", token);


		    //budget
		   track_tokenizer = subTokenize(line, token,track_tokenizer);
			trim(token);
		   data.budget = atoi(token);
	
		  // printf("%d\n", atoi(token));

		   //title_year
		   track_tokenizer = subTokenize(line, token,track_tokenizer);
		trim(token);
		   data.title_year =atoi( token);
		  // printf("%d\n", atoi(token));

		   // actor_2_facebook_likes
		    track_tokenizer = subTokenize(line, token,track_tokenizer);
		trim(token);
		    data.actor_2_facebook_likes = atoi(token);
		   // printf("%d\n", atoi(token));

		    //imdb_score
		    track_tokenizer = subTokenize(line, token,track_tokenizer);
		trim(token);
		    data.imdb_score = atof(token);
		    // printf("%d\n", atoi(token));

		     //aspect_ratio
		    track_tokenizer = subTokenize(line, token,track_tokenizer);
		trim(token);
		    data.aspect_ratio = atof(token);
		    // printf("%d\n", atoi(token));

		    //movie facebook _like
		    track_tokenizer = subTokenize(line, token,track_tokenizer);
		trim(token);
		    data.movie_facebook_likes = atoi(token);
		    // printf("%d\n", atoi(token));


		    track_tokenizer= 0;
		    //line[0]= '\0';


		    temp_mvs[temp_total_num_of_movies] = data;
		    temp_total_num_of_movies +=1;

		  // printf("the line number: %d\n", movie_number);

		
		//printf("%s\n", buffer);
		//fprintf(file, "%s\n", buffer);
		bzero(line , 1000);


		write(clientfd, "ok",2);
	

	}
	bzero(line , 1000);
	write(clientfd, "ok",2);

	read(clientfd, line , 1000);
	//printf("sortby is: %s\n", line);

	mergeSort(temp_mvs, temp_total_num_of_movies, line);
	
	printMovies(temp_mvs, temp_total_num_of_movies,"w",file_name);
	
	
/*--------------------------------------SENDING THE DATA BACK--------------------------------*/

	int client_ID =-1 ;
	read(clientfd, &client_ID, 4);
	//printf("client id is: %d\n", client_ID);


	FILE * file = fopen(file_name, "r");
	if (file == NULL){


		printf("erorr opening the file:\n%s", file_name);
		exit(0);
	}


	bzero(line,1000);
	fgets(line,1000,file); //getting the first line

	 while (strlen(line)>1){

		
		write(clientfd , line, strlen(line));
		
		read(clientfd , line , 5);

		bzero(line ,1000);
	        fgets(line,1000,file);

	}

	bzero(line, 1000);
	write(clientfd , "done", 4);

	if(remove(file_name) != 0){


		printf("problem removing a file: %s\n", file_name);
	}

	return ;
	

}


int subTokenize(char first[],char second[], int index){
    int i=0 ;

    if (first[index] =='"'){

		
	    second[i] = '"'; //putting " for the first character
	    i++;
            index++; // getting after "

            while (first[index]!='"'&& first[index]!= '\0'&& first[index]!= '\n'){ //until the next one comes

                second[i]= first[index];
                index++;
                i++;
            }
            second[i]= '"'; // putting " in the last character
	     second[i+1] = '\0';

	index = index+2;
        return index;
    }
    else{

        while (first[index]!= ','&& first[index]!= '\0'&& first[index]!= '\n'){

            second[i] = first[index];
            index++;
            i++;

        }
        second[i] = '\0';
	index = index+1;
        return index;
    }
}





char* trim (char* input){


    //checking if the input is Null or contains nothing
    //return: Null
    if (input == NULL || strlen(input)<1){

        return input;
    }

    //variable i starts with the last letter of the word and proceeds backwards until hit an actual letter
    int i, j=0;
    i= strlen(input) -1;

    //executes until it hits a valid letter
    while ((input[i]== ' '|| input[i]== '\n')&& i>=0){

        i--;
    }
    // no spaces found
    //return : input
    if (i == strlen(input)-1){
//input[strlen(input)+1] = '\0';
        return input;
    }
    input[i+1] = '\0';
    /** got rid of all the trailing zero at this time**/



     /** starting from the start of string and removing all the leading zero**/
     while (input[j]== ' ' ){


        j++;
     }

     if (!(j>0)){

        return input;
     }

     int index=0;

     while (j<=i){

        input[index] = input[j];
        j++;
        index++;
     }
     input[index] = '\0';


    return input;



}


void printMovies(Movie  movies[], int total_movies, char command[],char output_name[]){

	FILE* outputName = fopen(output_name, command);
	
	if (outputName == NULL){

		printf("PRINTMOVIES..COULD NOT OPEN THE FILE: %s\n",output_name);
		fclose(outputName);
		return ;
	}
	
	if (strcmp(command,"w")==0){
	//fprintf(outputName, "%s",header);
	//printf("%s",header);
	}
	

	int i= 0 ;
	Movie movie;
	Movie movie2;

	for (i=0 ; i< total_movies ; i++){

	movie = movies[i];
	
	i++;
	
	if ( i < total_movies){


		movie2 = movies[i];

	
		fprintf(outputName,"%s,%s,%d,%d,%d,%d,%s,%d,%d,%s,%s,%s,%d,%d,%s,%d,%s,%s,%d,%s,%s,%s,%d,%d,%d,%f,%f,%d\n%s,%s,%d,%d,%d,%d,%s,%d,%d,%s,%s,%s,%d,%d,%s,%d,%s,%s,%d,%s,%s,%s,%d,%d,%d,%f,%f,%d\n" , movie.color, movie.director_name,movie.num_critic_for_reviews,movie.duration,movie.director_facebook_likes,
movie.actor_3_facebook_likes,movie.actor_2_name,movie.actor_1_facebook_likes,movie.gross,movie.genres,
movie.actor_1_name,movie.movie_title,movie.num_voted_users,movie.cast_total_facebook_likes,movie.actor_3_name,movie.facenumber_in_poster,
movie.plot_keywords,movie.movie_imdb_link,movie.num_user_for_reviews,movie.language,movie.country,movie.content_rating,movie.budget,
movie.title_year,
movie.actor_2_facebook_likes,movie.imdb_score,movie.aspect_ratio,movie.movie_facebook_likes,

movie2.color, movie2.director_name,movie2.num_critic_for_reviews,movie2.duration,movie2.director_facebook_likes,
movie2.actor_3_facebook_likes,movie2.actor_2_name,movie2.actor_1_facebook_likes,movie2.gross,movie2.genres,
movie2.actor_1_name,movie2.movie_title,movie2.num_voted_users,movie2.cast_total_facebook_likes,movie2.actor_3_name,movie2.facenumber_in_poster,
movie2.plot_keywords,movie2.movie_imdb_link,movie2.num_user_for_reviews,movie2.language,movie2.country,movie2.content_rating,movie2.budget,
movie2.title_year,
movie2.actor_2_facebook_likes,movie2.imdb_score,movie2.aspect_ratio,movie2.movie_facebook_likes
);


	}else {

		
		fprintf(outputName,"%s,%s,%d,%d,%d,%d,%s,%d,%d,%s,%s,%s,%d,%d,%s,%d,%s,%s,%d,%s,%s,%s,%d,%d,%d,%f,%f,%d\n" , movie.color, movie.director_name,movie.num_critic_for_reviews,movie.duration,movie.director_facebook_likes,
movie.actor_3_facebook_likes,movie.actor_2_name,movie.actor_1_facebook_likes,movie.gross,movie.genres,
movie.actor_1_name,movie.movie_title,movie.num_voted_users,movie.cast_total_facebook_likes,movie.actor_3_name,movie.facenumber_in_poster,
movie.plot_keywords,movie.movie_imdb_link,movie.num_user_for_reviews,movie.language,movie.country,movie.content_rating,movie.budget,
movie.title_year,
movie.actor_2_facebook_likes,movie.imdb_score,movie.aspect_ratio,movie.movie_facebook_likes);
		

	}
	


	/*fprintf(outputName,"%s,%s,%d,%d,%d,%d,%s,%d,%d,%s,%s,%s,%d,%d,%s,%d,%s,%s,%d,%s,%s,%s,%d,%d,%d,%f,%f,%d\n" , movie.color, movie.director_name,movie.num_critic_for_reviews,movie.duration,movie.director_facebook_likes,
movie.actor_3_facebook_likes,movie.actor_2_name,movie.actor_1_facebook_likes,movie.gross,movie.genres,
movie.actor_1_name,movie.movie_title,movie.num_voted_users,movie.cast_total_facebook_likes,movie.actor_3_name,movie.facenumber_in_poster,
movie.plot_keywords,movie.movie_imdb_link,movie.num_user_for_reviews,movie.language,movie.country,movie.content_rating,movie.budget,
movie.title_year,
movie.actor_2_facebook_likes,movie.imdb_score,movie.aspect_ratio,movie.movie_facebook_likes);

*/
	/*printf("%s,%s,%d,%d,%d,%d,%s,%d,%d,%s,%s,%s,%d,%d,%s,%d,%s,%s,%d,%s,%s,%s,%d,%d,%d,%f,%f,%d\n" , movie.color, movie.director_name,movie.num_critic_for_reviews,movie.duration,movie.director_facebook_likes,
movie.actor_3_facebook_likes,movie.actor_2_name,movie.actor_1_facebook_likes,movie.gross,movie.genres,
movie.actor_1_name,movie.movie_title,movie.num_voted_users,movie.cast_total_facebook_likes,movie.actor_3_name,movie.facenumber_in_poster,
movie.plot_keywords,movie.movie_imdb_link,movie.num_user_for_reviews,movie.language,movie.country,movie.content_rating,movie.budget,
movie.title_year,
movie.actor_2_facebook_likes,movie.imdb_score,movie.aspect_ratio,movie.movie_facebook_likes);*/

	

	}

fclose(outputName);


}




int compare_movie(Movie movie1,Movie movie2, char sort_by[]){

	int output;

	if (strcmp(sort_by,"color")==0){

		/** Quotation marks checking**/
		
		//for movies 1
		char first[2];
		first[0] = movie1.color[0] == '"' ? movie1.color[1] : movie1.color[0];
		first[1] = '\0';

		//for movie 2
		char second[2];
		second[0] = movie2.color[0]=='"'? movie2.color[1] : movie2.color[0];
		second[1] = '\0';
		

		output = strcasecmp(second,first);

		//output = strcasecmp(movie2.color,movie1.color);
	
	}

	else if (strcmp(sort_by,"director_name")==0){

		//for movies 1
		char first[50];
		int i;
		int num=0;	
		for(i=0; i<=strlen(movie1.director_name);i++){
			if(movie1.director_name[i]=='"'){continue;}
			first[num]=movie1.director_name[i];
			num++;
		}
		
		//for movie 2
		char second[50];
		num=0;
		
		for(i=0; i<=strlen(movie2.director_name);i++){
			if(movie2.director_name[i]=='"'){continue;}
			second[num]=movie2.director_name[i];
			num++;
		}

		

		int len_1 = strlen(first);
		int len_2 = strlen(second);
		output = compareString(first,second,len_1,len_2);

 		//output = strcasecmp(movie2.director_name,movie1.director_name);
	}


	else if (strcmp(sort_by,"num_critic_for_reviews")==0){

 		output = movie1.num_critic_for_reviews >= movie2.num_critic_for_reviews? -1: 1;
	}
	

	else if (strcmp(sort_by,"duration")==0){

 		output = movie1.duration >= movie2.duration? -1: 1;
	}



	else if (strcmp(sort_by,"director_facebook_likes")==0){

 		output = movie1.director_facebook_likes >= movie2.director_facebook_likes? -1: 1;
	}


	else if (strcmp(sort_by,"actor_3_facebook_likes")==0){

 		output = movie1.actor_3_facebook_likes >= movie2.actor_3_facebook_likes? -1: 1;
	}


	else if (strcmp(sort_by,"actor_2_name")==0){

		/** Quotation marks checking**/
		
		//for movies 1
		char first[50];
		int i;
		int num=0;	
		for(i=0; i<=strlen(movie1.actor_2_name);i++){
			if(movie1.actor_2_name[i]=='"'){continue;}
			first[num]=movie1.actor_2_name[i];
			num++;
		}
		
		//for movie 2
		char second[50];
		num=0;
		
		for(i=0; i<=strlen(movie2.actor_2_name);i++){
			if(movie2.actor_2_name[i]=='"'){continue;}
			second[num]=movie2.actor_2_name[i];
			num++;
		}

		

		int len_1 = strlen(first);
		int len_2 = strlen(second);
		output = compareString(first,second,len_1,len_2);


	}

	else if (strcmp(sort_by,"actor_1_facebook_likes")==0){

 		output = movie1.actor_1_facebook_likes>= movie2.actor_1_facebook_likes? -1: 1;
	}


	else if (strcmp(sort_by,"gross")==0){

 		output = movie1.gross>= movie2.gross? -1: 1;
	}



	else if (strcmp(sort_by,"genres")==0){

		/** Quotation marks checking**/
		//for movies 1
		char first[100];
		int i;
		int num=0;	
		for(i=0; i<=strlen(movie1.genres);i++){
			if(movie1.genres[i]=='"'){continue;}
			first[num]=movie1.genres[i];
			num++;
		}
		
		//for movie 2
		char second[100];
		num=0;
		
		for(i=0; i<=strlen(movie2.genres);i++){
			if(movie2.genres[i]=='"'){continue;}
			second[num]=movie2.genres[i];
			num++;
		}

		

		int len_1 = strlen(first);
		int len_2 = strlen(second);
		output = compareString(first,second,len_1,len_2);
	

	}


	else if (strcmp(sort_by,"actor_1_name")==0){

		/** Quotation marks checking**/
		
		//for movies 1
		char first[50];
		int i;
		int num=0;	
		for(i=0; i<=strlen(movie1.actor_1_name);i++){
			if(movie1.actor_1_name[i]=='"'){continue;}
			first[num]=movie1.actor_1_name[i];
			num++;
		}
		
		//for movie 2
		char second[50];
		num=0;
		
		for(i=0; i<=strlen(movie2.actor_1_name);i++){
			if(movie2.actor_1_name[i]=='"'){continue;}
			second[num]=movie2.actor_1_name[i];
			num++;
		}

		

		int len_1 = strlen(first);
		int len_2 = strlen(second);
		output = compareString(first,second,len_1,len_2);

	}

	else if (strcmp(sort_by,"movie_title")==0){

		/** Quotation marks checking**/
		
		//for movies 1
		char first[50];
		int i;
		int num=0;	
		for(i=0; i<=strlen(movie1.movie_title);i++){
			if(movie1.movie_title[i]=='"'){continue;}
			first[num]=movie1.movie_title[i];
			num++;
		}
		
		//for movie 2
		char second[50];
		num=0;
		
		for(i=0; i<=strlen(movie2.movie_title);i++){
			if(movie2.movie_title[i]=='"'){continue;}
			second[num]=movie2.movie_title[i];
			num++;
		}
		/*
		if(isdigit(first[0]) && isdigit(second[0])){
			int j =0;
			int num1=0;
			int num2=0;
			while(isdigit(first[j])||first[j]==','){
				
				if(first[j]==','){
					j++;
					continue;
				}
				num1 *=10;
				num1 += first[j] - '0';
				j++;
			}
			
			int p=0;
			while(isdigit(second[p])||second[p]==','){
				
				if(second[p]==','){
					p++;
					continue;
				}
				num2 *=10;
				num2 += second[p] - '0';
				p++;
			}
			
			if(num1>num2){
				output = -1;
			}else if (num1<num2){
				output = 1;
			}else{
				char first_sub[50];
				char second_sub[50];
				int count;
				int ptr = 0;
				for(count = j;count<=strlen(first);count++){
					first_sub[ptr] = first[count];
					ptr++;
				}
				ptr=0;
				for(count = p;count<strlen(second);count++){
					second_sub[ptr] = second[count];
					ptr++;
				}
				int len_1 = strlen(first_sub);
				int len_2 = strlen(second_sub);
				output = compareString(first_sub,second_sub,len_1,len_2);
			}
		}else{
			int len_1 = strlen(first);
			int len_2 = strlen(second);
			output = compareString(first,second,len_1,len_2);
			
		}*/
			int len_1 = strlen(first);
			int len_2 = strlen(second);
			output = compareString(first,second,len_1,len_2);
			
	}

	

	else if (strcmp(sort_by,"num_voted_users")==0){

 		output = movie1.num_voted_users >= movie2.num_voted_users ? -1: 1;
	}



	else if (strcmp(sort_by,"cast_total_facebook_likes")==0){

 		output = movie1.cast_total_facebook_likes >= movie2.cast_total_facebook_likes ? -1: 1;
	}



	else if (strcmp(sort_by,"actor_3_name")==0){

		/** Quotation marks checking**/
		
		//for movies 1
		char first[50];
		int i;
		int num=0;	
		for(i=0; i<=strlen(movie1.actor_3_name);i++){
			if(movie1.actor_3_name[i]=='"'){continue;}
			first[num]=movie1.actor_3_name[i];
			num++;
		}
		
		//for movie 2
		char second[50];
		num=0;
		
		for(i=0; i<=strlen(movie2.actor_3_name);i++){
			if(movie2.actor_3_name[i]=='"'){continue;}
			second[num]=movie2.actor_3_name[i];
			num++;
		}
		
		int len_1 = strlen(first);
		int len_2 = strlen(second);
		output = compareString(first,second,len_1,len_2);
	}

	
	else if (strcmp(sort_by,"facenumber_in_poster")==0){

 		output = movie1.facenumber_in_poster >= movie2.facenumber_in_poster ? -1: 1;
	}




	else if (strcmp(sort_by,"plot_keywords")==0){

		/** Quotation marks checking**/
		
		//for movies 1
		char first[500];
		int i;
		int num=0;	
		for(i=0; i<=strlen(movie1.plot_keywords);i++){
			if(movie1.plot_keywords[i]=='"'){continue;}
			first[num]=movie1.plot_keywords[i];
			num++;
		}
		
		//for movie 2
		char second[500];
		num=0;
		
		for(i=0; i<=strlen(movie2.plot_keywords);i++){
			if(movie2.plot_keywords[i]=='"'){continue;}
			second[num]=movie2.plot_keywords[i];
			num++;
		}

		int len_1 = strlen(first);
		int len_2 = strlen(second);
		output = compareString(first,second,len_1,len_2);

	}

	else if (strcmp(sort_by,"movie_imdb_link")==0){

		/** Quotation marks checking**/
		
		//for movies 1
		char first[100];
		int i;
		int num=0;	
		for(i=0; i<=strlen(movie1.movie_imdb_link);i++){
			if(movie1.movie_imdb_link[i]=='"'){continue;}
			first[num]=movie1.movie_imdb_link[i];
			num++;
		}
		
		//for movie 2
		char second[100];
		num=0;
		
		for(i=0; i<=strlen(movie2.movie_imdb_link);i++){
			if(movie2.movie_imdb_link[i]=='"'){continue;}
			second[num]=movie2.movie_imdb_link[i];
			num++;
		}
		

		int len_1 = strlen(first);
		int len_2 = strlen(second);
		output = compareString(first,second,len_1,len_2);

	}


	
	else if (strcmp(sort_by,"num_user_for_reviews")==0){

		

 		output = movie1.num_user_for_reviews >= movie2.num_user_for_reviews ? -1: 1;
	}



	else if (strcmp(sort_by,"language")==0){

		/** Quotation marks checking**/
		
		//for movies 1
		char first[50];
		int i;
		int num=0;	
		for(i=0; i<=strlen(movie1.language);i++){
			if(movie1.language[i]=='"'){continue;}
			first[num]=movie1.language[i];
			num++;
		}
		
		//for movie 2
		char second[50];
		num=0;
		
		for(i=0; i<=strlen(movie2.language);i++){
			if(movie2.language[i]=='"'){continue;}
			second[num]=movie2.language[i];
			num++;
		}
		int len_1 = strlen(first);
		int len_2 = strlen(second);
		output = compareString(first,second,len_1,len_2);

	}


	else if (strcmp(sort_by,"country")==0){
		
		/** Quotation marks checking**/
		
		//for movies 1
		char first[50];
		int i;
		int num=0;	
		for(i=0; i<=strlen(movie1.country);i++){
			if(movie1.country[i]=='"'){continue;}
			first[num]=movie1.country[i];
			num++;
		}
		
		//for movie 2
		char second[50];
		num=0;
		
		for(i=0; i<=strlen(movie2.country);i++){
			if(movie2.country[i]=='"'){continue;}
			second[num]=movie2.country[i];
			num++;
		}
		int len_1 = strlen(first);
		int len_2 = strlen(second);
		output = compareString(first,second,len_1,len_2);

	}

	else if (strcmp(sort_by,"content_rating")==0){

		/** Quotation marks checking**/
		
		//for movies 1
		char first[50];
		int i;
		int num=0;	
		for(i=0; i<=strlen(movie1.content_rating);i++){
			if(movie1.content_rating[i]=='"'){continue;}
			first[num]=movie1.content_rating[i];
			num++;
		}
		
		//for movie 2
		char second[50];
		num=0;
		
		for(i=0; i<=strlen(movie2.content_rating);i++){
			if(movie2.content_rating[i]=='"'){continue;}
			second[num]=movie2.content_rating[i];
			num++;
		}
		int len_1 = strlen(first);
		int len_2 = strlen(second);
		output = compareString(first,second,len_1,len_2);

	}

	else if (strcmp(sort_by,"budget")==0){

 		output = movie1.budget >= movie2.budget ? -1: 1;
	}
	

	else if (strcmp(sort_by,"title_year")==0){

 		output = movie1.title_year >= movie2.title_year ? -1: 1;
	}
	

	else if (strcmp(sort_by,"actor_2_facebook_likes")==0){

 		output = movie1.actor_2_facebook_likes >= movie2.actor_2_facebook_likes ? -1: 1;
	}
	

	else if (strcmp(sort_by,"imdb_score")==0){

 		output = movie1.imdb_score >= movie2.imdb_score ? -1: 1;
	}
	

	else if (strcmp(sort_by,"aspect_ratio")==0){

 		output = movie1.aspect_ratio >= movie2.aspect_ratio ? -1: 1;
	}


	else if (strcmp(sort_by,"movie_facebook_likes")==0){

 		output = movie1.movie_facebook_likes >= movie2.movie_facebook_likes ? -1: 1;
	}

	else{


	printf("Wrong Parameter\n");
	exit(0);

	}


	return output;
	


}



int compareString(char*,char*,int, int);
int compareString(char* first, char* second, int len_1, int len_2){
	int output = 0;
	int fix_len= len_2>=len_1 ? len_1 : len_2;
	int iter=0;
	
		while(iter<=fix_len){
			if(first[iter]!=second[iter]){
				if(first[iter]>second[iter]){
					output = -1;
					break;
				}else{
					output = 1;
					break;
				}
			}
			iter++;
		}
		
		if(iter==fix_len){
			if(iter<len_1){
				output = -1;
			}else if(iter<len_2){
				output = 1;
			}else{
				output = 0;
				}
		}
		
	return output;
}
