namespace fes{
    int checksum(unsigned char myarray[], int m){  // checksum is a function that preforms checksums of all of the byte strings used in this code
        int csum = 0;
        for (int i = 0; i < m - 1; i++) {
            csum += myarray[i];
        }
        csum = ((0x00FF & csum) + (csum >> 8)) ^ 0xFF;
        return csum;
    }
}
