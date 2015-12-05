#!env/bin/python
import urllib2
import simplejson
import datetime

APP_ID = "40639356d56148f1ae26348d670e889f"
TARGET_URL = "http://taggy-api.bx23.net/api/v1/currency/"

def main():
    print 'Getting rates...'
    request = urllib2.Request("http://openexchangerates.org/api/latest.json?app_id=%s" % (APP_ID))
    opener = urllib2.build_opener()
    f = opener.open(request)
    result = simplejson.load(f)
    rates = result['rates']
    date = datetime.datetime.fromtimestamp(int(result['timestamp']))
    print 'Rates [%s] size: %s' % (date, len(rates))

    print 'Sending to API...'
    update_j = {"currency" : [], "timestamp" : result['timestamp']}
    for name, value in rates.iteritems():
        update_j["currency"].append({"name" : name, "value" : value})

    request = urllib2.Request(TARGET_URL, simplejson.dumps(update_j), {'Content-Type': 'application/json'})
    f = urllib2.urlopen(request)
    response = f.read()
    f.close()
    print ' API: %s' % (response)

if __name__ == '__main__':
    main()
