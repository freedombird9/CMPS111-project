#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "rijndael.h"

#define KEYBITS 128

int hexvalue (char c)
{
  if (c >= '0' && c <= '9') {
    return (c - '0');
  } else if (c >= 'a' && c <= 'f') {
    return (10 + c - 'a');
  } else if (c >= 'A' && c <= 'F') {
    return (10 + c - 'A');
  } else {
    fprintf (stderr, "ERROR: key digit %c isn't a hex digit!\n", c);
    exit (-1);
  }
}

void getpassword (const char *password, unsigned char *key, int keylen)
{
  int		i;
  
  for (i = 0; i < keylen; i++) {
    if (*password == '\0') {
      key[i] = 0;
    } else {
      /* Add the first of two digits to the current key value */
      key[i] = hexvalue (*(password++)) << 4;
      /* If there's a second digit at this position, add it */
      if (*password != '\0') {
		key[i] |= hexvalue (*(password++));
      }
    }
  }
}


int main(int argc, char **argv){
  unsigned long rk[RKLENGTH(KEYBITS)];		/* round key */
  unsigned char key[KEYLENGTH(KEYBITS)];	/* cipher key */
  char	buf[100];
  int i, nbytes, nwritten , ctr;
  int totalbytes;
  int	k0, k1;
  int fileId;
  int nrounds;					/* # of Rijndael rounds */
  char *password;				/* supplied (ASCII) password */
  int	fd;
  const char *filename;
  unsigned char filedata[16];
  unsigned char ciphertext[16];
  unsigned char ctrvalue[16];
  int mode;
  char unset[] = "0664";
  struct stat status;

  if (argc < 4)
  {
    fprintf (stderr, "Usage: %s <e/d> <key> <filename>\n", argv[0]);
    return 1;
  }

  filename = argv[3];
  getpassword (argv[2], key, sizeof (key));

  if( stat(filename, &status) != 0 ){
    fprintf(stderr, "error occurred getting the stat of the file\n");
    return 1;
  }

  fd = open(filename, O_RDWR);

  if (fd < 0){	
	fprintf(stderr, "Error opening file %s\n", argv[2]);
	return 1;
  }
  
  if (*argv[1] == 'd') {     /* set off the sticky bit, no longer need encryption */
	if( !(status.st_mode & S_ISVTX) ){
	  fprintf(stderr, "decryption error: file is not encrypted\n");
	  return 1;
	}
    mode = strtol(unset, 0, 8);
    if ( chmod(filename, mode) != 0 ){
      fprintf(stderr, "error occurred while setting off the sticky bit\n");
      return 1;
    }    
  }
  
  else if (*argv[1] == 'e'){
	if (status.st_mode & S_ISVTX){   /* if sticky bit is already set */
	  fprintf(stderr, "encryption error: sticky bit is already set\n");
	  return 1;
	}
    if ( chmod(filename, S_ISVTX|S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH ) != 0){    /* otherwise, set the sticky bit */
      fprintf(stderr, "error occureed while setting on the sticky bit\n");
      return 1;
    }
  }
  
  nrounds = rijndaelSetupEncrypt(rk, key, KEYBITS);
  
  fileId = status.st_ino;    /* get the i-node number */
  
  /* fileID goes into bytes 8-11 of the ctrvalue */
  bcopy (&fileId, &(ctrvalue[8]), sizeof (fileId));
  
  /* This loop reads 16 bytes from the file, XORs it with the encrypted
	 CTR value, and then writes it back to the file at the same position.
	 Note that CTR encryption is nice because the same algorithm does
	 encryption and decryption.  In other words, if you run this program
	 twice, it will first encrypt and then decrypt the file.
  */
  for (ctr = 0, totalbytes = 0; /* loop forever */; ctr++){
	
	/* Read 16 bytes (128 bits, the blocksize) from the file */
	nbytes = read (fd, filedata, sizeof (filedata));
	if (nbytes <= 0) {
	  break;
	}
	if (lseek (fd, totalbytes, SEEK_SET) < 0)
	  {
		perror ("Unable to seek back over buffer");
		exit (-1);
	  }
	
	/* Set up the CTR value to be encrypted */
	bcopy (&ctr, &(ctrvalue[0]), sizeof (ctr));
	
	/* Call the encryption routine to encrypt the CTR value */
	rijndaelEncrypt(rk, nrounds, ctrvalue, ciphertext);
	
	/* XOR the result into the file data */
	for (i = 0; i < nbytes; i++) {
	  filedata[i] ^= ciphertext[i];
	}
	
	/* Write the result back to the file */
	nwritten = write(fd, filedata, nbytes);
	if (nwritten != nbytes)
	  {
		fprintf (stderr,
				   "%s: error writing the file (expected %d, got %d at ctr %d\n)",
				 argv[0], nbytes, nwritten, ctr);
		  break;
	  }
	
	/* Increment the total bytes written */
	totalbytes += nbytes;
  }   /* end for */ 
  
  close(fd);
  return 0;
}
