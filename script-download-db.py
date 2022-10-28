import requests
import json
import time
import os

#bearer_token = ("AAAAAAAAAAAAAAAAAAAAAG03gQEAAAAA9V94CJOFuo4GnVIli61lfsyo4oI%3DfMzDJRrucDrFG5jRxsj25CALLT6se7Nby9EvZFixMBiAJJiF65")
bearer_token = ("AAAAAAAAAAAAAAAAAAAAAOGSgwEAAAAAmLmS3jatMxCnxAuCKG6Fmm%2Bsqhk%3DAGlafipaHUaUIhWBFeCFPRjmauoihqguXibVdih0IYu27sBovK")

search_url = "https://api.twitter.com/2/tweets/search/all"

query_params = {'max_results': 500, 'query': 'place_country:BR', 'until_id': '1554620879413313538'}

file_path = os.getcwd()+'/tweets/'


def bearer_oauth(r):
    r.headers["Authorization"] = f"Bearer "+bearer_token 
    r.headers["User-Agent"] = "v2FullArchiveSearchPython"
    return r

def connect_to_endpoint(url, params):
    response = requests.get(url, auth=bearer_oauth, params=params)
    print(response.status_code)
    if response.status_code != 200:
        raise Exception(response.status_code, response.text)
    return response.json()

def save_to_file(tweets):
    for tweet in tweets:
        file = open(file_path+str(tweet['id'])+'.txt', "w")
        file.write(tweet['text'])
        file.close()

def main():
    count = 0
    while count < 7000000:
        countToDelay120 = 0
        while countToDelay120 < 50000:
            countToDelay60 = 0
            while countToDelay60 < 5000:
                json_response = connect_to_endpoint(search_url, query_params)
                json_obj = json.loads(json.dumps(json_response, indent=4, sort_keys=True))
                print(json.dumps(json_response, indent=4, sort_keys=True))
                save_to_file(json_obj['data'])
                countToDelay60 += json_obj['meta']['result_count']
                query_params['next_token'] = json_obj['meta']['next_token']
            time.sleep(90)
            countToDelay120 += countToDelay60
        time.sleep(120)
        count += countToDelay120

main()