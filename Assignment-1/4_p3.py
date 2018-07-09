# 4(a) pending. Need to read explicitly the aws credentials from a file
# how to run the program
# file "awskeys" with credentials are attached with the distribution
# need to install python 3 to run script properly
# python 4_p3.py

import boto3
import botocore
import os
import configparser

BUCKET_NAME = 'mashiur' # replace with your bucket name
KEY = '332.cpp' # replace with your object key
FILE_PATH = 'download.txt'
INPUT_PATH = "user_name.txt"
U_KEY = "assignment_1"

'''
session = boto3.Session(
    aws_access_key_id=settings.AWS_SERVER_PUBLIC_KEY,
    aws_secret_access_key=settings.AWS_SERVER_SECRET_KEY,
)
'''


config = configparser.ConfigParser()
# name of the credential file.

config.read('./awskeys')

access_key_id = config['default']['aws_access_key_id']
secret_key = config['default']['aws_secret_access_key']


session = boto3.Session(
    aws_access_key_id=access_key_id,
    aws_secret_access_key=secret_key,
)

s3 = session.resource('s3')
s3_client =  session.client('s3');

def delete_file_s3(bucket_name, key) :
	# do we need any error handling here
	s3.Object(bucket_name, key).delete()
	print("deleted successfully from  s3")

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
	print("downloaded successfully from s3")


def upload_s3(file_path, bucket_name, key_name) :
	# Upload tmp.txt to bucket-name at key-name
	s3_client.upload_file(file_path, bucket_name, key_name)
	# do i need to do any kind of error handling here
	print("uploaded  successfully to s3")

def get_IO() :
	#name = raw_input("Enter your name: ")   # Python 2.x
	name = input("Enter your name: ")	# Python 3

	return name;

def main() :

	name  = get_IO()
	write_file(INPUT_PATH, name)
	upload_s3(INPUT_PATH, BUCKET_NAME,U_KEY)	

	try:
		# s3.Bucket(BUCKET_NAME).download_file(KEY, FILE_PATH)
		download_from_s3(BUCKET_NAME, U_KEY, FILE_PATH)
		print("...........................")
		print("Content found inside the file is ")
		print(read_file(FILE_PATH))
		delete_file_s3(BUCKET_NAME, U_KEY)
		os.remove(FILE_PATH)

	except botocore.exceptions.ClientError as e:
		if e.response['Error']['Code'] == "404":
			print("The object does not exist.")
		else:
			raise


main()
