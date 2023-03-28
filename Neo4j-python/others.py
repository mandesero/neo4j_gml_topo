import time

def timer(func):
    def wrapped(*args, **kwargs):
        t0 = time.time()
        res = func(*args, **kwargs)
        return (time.time() - t0) * 1000, res
    return wrapped