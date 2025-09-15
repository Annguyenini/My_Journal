cache = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
newcache =[1, 2, 3, 5 , 7, 8, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20]


for fn1 in cache[:]:
    print(fn1)
    if fn1 not in newcache:
        cache.remove(fn1)
    
    elif fn1 in newcache:
        newcache.remove(fn1)

print (cache)
print (newcache)