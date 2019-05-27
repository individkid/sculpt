{-# LANGUAGE ForeignFunctionInterface #-}

module Main where

import Foreign.C
import Foreign.Ptr
import System.Environment
import AffTopo.Naive

foreign import ccall setDebug :: CInt -> IO ()
foreign import ccall clearDebug :: CInt -> IO ()
foreign import ccall enumerate :: Ptr CChar -> IO CInt
foreign import ccall rdPointer :: CInt -> IO (Ptr ())
foreign import ccall rdOpcode :: CInt -> IO CInt
foreign import ccall rdConfigure :: CInt -> IO CInt
foreign import ccall rdTopologyMode :: CInt -> IO CInt
foreign import ccall rdFixedMode :: CInt -> IO CInt
foreign import ccall rdSubconf :: CInt -> IO CInt
foreign import ccall rdInt :: CInt -> IO CInt
foreign import ccall rdFloat :: CInt -> IO CFloat
foreign import ccall rdDouble :: CInt -> IO CDouble
foreign import ccall rdChars :: CInt -> CInt -> Ptr CChar -> IO ()
foreign import ccall rdInts :: CInt -> CInt -> Ptr CInt -> IO ()
foreign import ccall rdFloats :: CInt -> CInt -> Ptr CFloat -> IO ()
foreign import ccall rdDoubles :: CInt -> CInt -> Ptr CDouble -> IO ()
foreign import ccall wrPointer :: CInt -> Ptr () -> IO ()
foreign import ccall wrOpcode :: CInt -> CInt -> IO ()
foreign import ccall wrConfigure :: CInt -> CInt -> IO ()
foreign import ccall wrTopologyMode :: CInt -> CInt -> IO ()
foreign import ccall wrFixedMode :: CInt -> CInt -> IO ()
foreign import ccall wrSubconf :: CInt -> CInt -> IO ()
foreign import ccall wrInt :: CInt -> CInt -> IO ()
foreign import ccall wrFloat :: CInt -> CFloat -> IO ()
foreign import ccall wrDouble :: CInt -> CDouble -> IO ()
foreign import ccall wrChars :: CInt -> CInt -> Ptr CChar -> IO ()
foreign import ccall wrInts :: CInt -> CInt -> Ptr CInt -> IO ()
foreign import ccall wrFloats :: CInt -> CInt -> Ptr CFloat -> IO ()
foreign import ccall wrDoubles :: CInt -> CInt -> Ptr CDouble -> IO ()
foreign import ccall exOpcode :: CInt -> CInt -> IO ()
foreign import ccall hello :: CString -> IO CString

main = do
   print (holes 5 [2,3,4])
   setDebug (toCInt 0x10000)
   [rdfd,wrfd] <- fmap (map read) getArgs
   testLoop rdfd wrfd

toInt :: Integral a => IO a -> IO Int
toInt = fmap (fromInteger . toInteger)

toCInt :: Integral a => a -> CInt
toCInt a = fromInteger (toInteger a)

testLoop :: CInt -> CInt -> IO ()
testLoop rdfd wrfd = do
   readOp <- (newCString "ReadOp") >>= enumerate
   fileOp <- (newCString "FileOp") >>= enumerate
   planeOp <- (newCString "PlaneOp") >>= enumerate
   confOp <- (newCString "ConfOp") >>= enumerate
   textOp <- (newCString "TextOp") >>= enumerate
   exOpcode rdfd readOp
   ptr <- rdPointer rdfd
   exOpcode rdfd fileOp
   file <- toInt (rdInt rdfd)
   exOpcode rdfd planeOp
   plane <- toInt (rdInt rdfd)
   exOpcode rdfd confOp
   conf <- toInt (rdConfigure rdfd)
   exOpcode rdfd textOp
   count <- toInt (rdInt rdfd)
   str <- newCString (replicate count '\0')
   rdChars rdfd (toCInt count) str
   (peekCString str) >>= print
   wrOpcode wrfd readOp
   wrPointer wrfd ptr
   testLoop rdfd wrfd
