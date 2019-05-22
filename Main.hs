{-# LANGUAGE ForeignFunctionInterface #-}

module Main where

import Prelude
import Foreign.C
import Foreign.Ptr
import System.Environment

foreign import ccall enumerate :: CString -> IO CInt
foreign import ccall identity :: CString -> IO CInt
foreign import ccall location :: CInt -> CString -> IO CInt
foreign import ccall structure :: CInt -> IO CInt
foreign import ccall integer :: CString -> CInt -> CInt -> IO (Ptr CInt)
foreign import ccall scalar :: CString -> CInt -> CInt -> IO (Ptr CFloat)
foreign import ccall text :: CString -> CInt -> CInt -> IO (Ptr CChar)
foreign import ccall fromenum :: CString -> CInt -> CInt -> IO CInt
foreign import ccall fromcount :: CString -> CInt -> CInt -> IO CInt
foreign import ccall toenum :: CString -> CInt -> CInt -> CInt -> IO ()
foreign import ccall tocount :: CString -> CInt -> CInt -> CInt -> IO ()
foreign import ccall reference :: CString -> CInt -> CInt -> CInt -> IO CInt
foreign import ccall extent :: CString -> CInt -> CInt -> IO CInt
foreign import ccall input :: CInt -> CInt -> CString -> IO ()
foreign import ccall output :: CInt -> CInt -> CString -> IO ()
foreign import ccall hello :: CString -> IO CString

main = do
   args <- getArgs
   print (map (\x -> ((read x)::Int)) args)
   (newCString "hello") >>= hello >>= peekCString >>= print
