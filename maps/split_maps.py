import sys

def main(file1, out1, out2):
    f = open(file1, "r")
    out1 = open(out1, "w")
    out2 = open(out2, "w")
    i = 0
    for line in f.readlines():
        if i < 256:
            out1.write("({0},{1},{2},{3},{4})".format(line[0], line[2] ,line[4], line[6], line[8]))
        else:
            out2.write("({0},{1},{2},{3},{4})".format(line[0], line[2] ,line[4], line[6], line[8]))
        i= i + 1

if __name__ == "__main__":
    for i in range(32):
        x = "comm-{0}".format(i)
        y = "split_maps/comm-{0}-0".format(i)
        z = "split_maps/comm-{0}-1".format(i)
        main(x,y,z)
