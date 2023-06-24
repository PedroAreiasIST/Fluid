PROGRAM determinesclovercd
  IMPLICIT REAL(8)(a-h,o-z)
  OPEN(12,file="lastrow.txt",status="unknown")
  OPEN(13,file="clovercd.txt",status="unknown")
  READ(12,*)time,rx,ry
  WRITE(13,"(12E15.3)")ry,ry/rx
  close(13)
END PROGRAM determinesclovercd
