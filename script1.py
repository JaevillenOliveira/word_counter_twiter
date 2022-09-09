import requests
import os
import csv
import json

bearer_token = ("AAAAAAAAAAAAAAAAAAAAAG03gQEAAAAA9V94CJOFuo4GnVIli61lfsyo4oI%3DfMzDJRrucDrFG5jRxsj25CALLT6se7Nby9EvZFixMBiAJJiF65")

search_url = "https://api.twitter.com/2/tweets/search/recent"

query_params = {'query': 'place_country:BR'}


def bearer_oauth(r):
    r.headers["Authorization"] = f"Bearer "+bearer_token 
    r.headers["User-Agent"] = "v2RecentSearchPython"
    return r

def connect_to_endpoint(url, params):
    response = requests.get(url, auth=bearer_oauth, params=params)
    print(response.status_code)
    if response.status_code != 200:
        raise Exception(response.status_code, response.text)
    return response.json()

def save_to_file(tweets):
    for tweet in tweets:
        with open(tweet['id']+'.csv', mode='w') as file:
            file = csv.writer(file)
            file.writerow([tweet['text']])

def main():
    json_response = connect_to_endpoint(search_url, query_params)
    json_obj = json.loads(json.dumps(json_response, indent=4, sort_keys=True))
    print(json.dumps(json_response, indent=4, sort_keys=True))
    save_to_file(json_obj['data'])


main()