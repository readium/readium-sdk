import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.IOException;

public final class contentFilterChainEncode
{
	public static void main(String[] args) throws Exception
    {
		System.out.print('\n');
		System.out.print("FILE PATH TO ENCODE:\n");
		System.out.print(args[0]);
		System.out.print('\n');
        
        String ORIGINAL = "_ORIGINAL";
        String backupFilePath = args[0] + ORIGINAL;
        int dotIndex = args[0].lastIndexOf(".");
        if (dotIndex > 0)
        {
            String ext = args[0].substring(dotIndex);
            String filePathWithoutExt = args[0].substring(0, dotIndex);
            backupFilePath = filePathWithoutExt + ORIGINAL + ext;
        }
        
        System.out.print('\n');
		System.out.print("BACKUP FILE PATH:\n");
		System.out.print(backupFilePath);
        System.out.print('\n');
        
        File backupFile = new File(backupFilePath);
        if (backupFile.exists()) backupFile.delete();
        
        File newFile = new File(args[0]);
        newFile.renameTo(backupFile);
        newFile = new File(args[0]);
        
        int BUFFER_SIZE = 1024 * 5; // kilo bytes
        byte[] byteBuffer = new byte[BUFFER_SIZE];
        
        int PADDING_BYTES = 3;
        
        int totalBytesRead = 0;
        int totalBytesWrite = 0;
        
        int nPads = 0;

		System.out.print("====== 1) ENCODING...\n");
        
        InputStream in = null;
        OutputStream out = null;
        try
        {
            in = new FileInputStream(backupFile);
            out = new FileOutputStream(newFile);
            
            int bytesRead;
            while((bytesRead = in.read(byteBuffer, 0, BUFFER_SIZE)) > 0)
            {
                totalBytesRead += bytesRead;
                
                out.write(byteBuffer, 0, bytesRead);
                totalBytesWrite += bytesRead;
                
                if (bytesRead < BUFFER_SIZE) continue;
                
                for (int i = 0; i < PADDING_BYTES; i++)
                {
                    byteBuffer[i] = 0;
                }
                out.write(byteBuffer, 0, PADDING_BYTES);
                totalBytesWrite += PADDING_BYTES;
                nPads++;
            }
        }
        catch (Exception ex)
        {
            ex.printStackTrace();
        }
        finally
        {
            if (in != null) in.close();
            if (out != null) out.close();
        }
        
		System.out.print('\n');
        System.out.print("INPUT FILE SIZE: " + backupFile.length() + '\n');
        System.out.print("INPUT TOTAL BYTES: " + totalBytesRead + '\n');
		System.out.print('\n');
        System.out.print("OUTPUT FILE SIZE: " + newFile.length() + '\n');
        System.out.print("OUTPUT TOTAL BYTES: " + totalBytesWrite + '\n');
		System.out.print('\n');
        System.out.print("TOTAL NUMBER OF BYTE PADDING ITERATIONS: " + nPads + '\n');
        System.out.print("INDIVIDUAL BYTE PADDING SIZE: " + PADDING_BYTES + '\n');
        System.out.print("ADDED PADDING (BYTES): " + (nPads * PADDING_BYTES) + '\n');
        System.out.print("SHOULD MATCH FILE SIZE DIFF: " + (newFile.length() - backupFile.length()) + '\n');

        
        
        System.out.print('\n');
        System.out.print('\n');
        
        
        String DECODED = "_DECODED";
        String decodedFilePath = args[0] + DECODED;
        dotIndex = args[0].lastIndexOf(".");
        if (dotIndex > 0)
        {
            String ext = args[0].substring(dotIndex);
            String filePathWithoutExt = args[0].substring(0, dotIndex);
            decodedFilePath = filePathWithoutExt + DECODED + ext;
        }
        
        System.out.print('\n');
		System.out.print("DECODED FILE PATH:\n");
		System.out.print(decodedFilePath);
        System.out.print('\n');
        
        File decodedFile = new File(decodedFilePath);
        if (decodedFile.exists()) decodedFile.delete();
        
        totalBytesRead = 0;
        totalBytesWrite = 0;
        nPads = 0;

		System.out.print("====== 2) DECODING...\n");
        
        in = null;
        out = null;
        try
        {
            in = new FileInputStream(newFile);
            out = new FileOutputStream(decodedFile);
            
            int bytesRead;
            while((bytesRead = in.read(byteBuffer, 0, BUFFER_SIZE)) > 0)
            {
                totalBytesRead += bytesRead;
                
                out.write(byteBuffer, 0, bytesRead);
                totalBytesWrite += bytesRead;
                
                if (bytesRead < BUFFER_SIZE) continue;
                
                bytesRead = in.read(byteBuffer, 0, PADDING_BYTES);
                
                if (bytesRead != PADDING_BYTES)
                {
                    System.err.print("DECODE ERROR (bytesRead != PADDING_BYTES): " + bytesRead + " != " + PADDING_BYTES + '\n');
                    throw new Exception("DECODE ERROR");
                }
                
                for (int i = 0; i < PADDING_BYTES; i++)
                {
                    if (byteBuffer[i] != 0)
                    {
                        System.err.print("DECODE ERROR (byteBuffer[i] != 0): " + i + '\n');
                        throw new Exception("DECODE ERROR");
                    }
                }
                
                nPads++;
            }
        }
        catch (Exception ex)
        {
            ex.printStackTrace();
        }
        finally
        {
            if (in != null) in.close();
            if (out != null) out.close();
        }
        
		System.out.print('\n');
        System.out.print("INPUT FILE SIZE: " + newFile.length() + '\n');
        System.out.print("INPUT TOTAL BYTES: " + totalBytesRead + '\n');
		System.out.print('\n');
        System.out.print("OUTPUT FILE SIZE: " + decodedFile.length() + '\n');
        System.out.print("OUTPUT TOTAL BYTES: " + totalBytesWrite + '\n');
		System.out.print('\n');
        System.out.print("TOTAL NUMBER OF BYTE PADDING ITERATIONS: " + nPads + '\n');
        System.out.print("INDIVIDUAL BYTE PADDING SIZE: " + PADDING_BYTES + '\n');
        System.out.print("REMOVED PADDING (BYTES): " + (nPads * PADDING_BYTES) + '\n');
        System.out.print("SHOULD MATCH FILE SIZE DIFF: " + (newFile.length() - decodedFile.length()) + '\n');
        

        System.out.print('\n');
		System.out.print("====== 3) COMPARING IDENTICAL...");

        byte[] byteBuffer2 = new byte[BUFFER_SIZE];
                
        InputStream in2 = null;
        try
        {
            in = new FileInputStream(backupFile);
            in2 = new FileInputStream(decodedFile);
            
            int bytesRead = 0;
            int bytesRead2 = 0;
            while(
                (bytesRead = in.read(byteBuffer, 0, BUFFER_SIZE)) > 0
                &&
                (bytesRead2 = in2.read(byteBuffer2, 0, BUFFER_SIZE)) > 0
            )
            {
                if (bytesRead != bytesRead2)
                {
                    System.err.print("\nDECODE ERROR #1 (bytesRead != bytesRead2): " + bytesRead + " != " + bytesRead2 + '\n');
                    throw new Exception("DECODE ERROR");
                }
                
                for (int i = 0; i < bytesRead; i++)
                {
                    if (byteBuffer[i] != byteBuffer2[i])
                    {
                        System.err.print("\nDECODE ERROR (byteBuffer[i] != byteBuffer2[i]): " + i + '\n');
                        throw new Exception("DECODE ERROR");
                    }
                }

        		System.out.print('.');
            }
            
            if (bytesRead > 0)
            {
                System.err.print("\nDECODE ERROR #2 (bytesRead != bytesRead2): " + bytesRead + " != " + bytesRead2 + '\n');
                throw new Exception("DECODE ERROR");
            }
            else
            {
                bytesRead2 = in2.read(byteBuffer2, 0, BUFFER_SIZE);
                
                if (bytesRead2 > 0)
                {
                    System.err.print("\nDECODE ERROR #3 (bytesRead != bytesRead2): " + bytesRead + " != " + bytesRead2 + '\n');
                    throw new Exception("DECODE ERROR");
                }
            }

    		System.out.print('OKAY.\n');
        }
        catch (Exception ex)
        {
            ex.printStackTrace();
        }
        finally
        {
            if (in != null) in.close();
            if (out != null) out.close();
        }

        System.out.print('\n');
		System.out.print("EXIT.\n");
    }
}