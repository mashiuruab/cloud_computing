import boto3
import botocore
import os

BUCKET_NAME = 'mashiur' # replace with your bucket name
KEY = '332.cpp' # replace with your object key
FILE_PATH = 'download.txt'
INPUT_PATH = "user_name.txt"
U_KEY = "assignment_1"

s3 = boto3.resource('s3')
s3_client =  boto3.client('s3');

def delete_file_s3(bucket_name, key) :
	# do we need any error handling here
	s3.Object(bucket_name, key).delete()
	print "deleted successfully from  s3"

def read_file(path) :
	file = open(FILE_PATH, "r")
	file_content = file.read()
	file.close()
	return file_content


def write_file(path, str_value) :
	file = open(path,"w")
	file.write(str_value)  
	file.close()

def download_from_s3(bucket_name, key, path) :
	s3.Bucket(bucket_name).download_file(key, path)
	print "downloaded successfully from s3"


def upload_s3(file_path, bucket_name, key_name) :
	# Upload tmp.txt to bucket-name at key-name
	s3_client.upload_file(file_path, bucket_name, key_name)
	# do i need to do any kind of error handling here
	print "uploaded  successfully to s3"

def get_IO() :
	name = raw_input("Enter your name: ")   # Python 2.x
	# name = input("Enter your name: ")	# Python 3

	return name;

def main() :

	name  = get_IO()
	write_file(INPUT_PATH, name)
	upload_s3(INPUT_PATH, BUCKET_NAME,U_KEY)	

	try:
		# s3.Bucket(BUCKET_NAME).download_file(KEY, FILE_PATH)
		download_from_s3(BUCKET_NAME, U_KEY, FILE_PATH)
		print "..........................."
		print "Content found inside the file is "
		print read_file(FILE_PATH)
		delete_file_s3(BUCKET_NAME, U_KEY)
		os.remove(FILE_PATH)

	except botocore.exceptions.ClientError as e:
		if e.response['Error']['Code'] == "404":
			print("The object does not exist.")
		else:
			raise


main()
