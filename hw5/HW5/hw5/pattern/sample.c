int result;
int n;
int fact()
{
    int ans;
    ans = 1;
    if (n == 1)
    {
	return 1;
    }
    else
    {
    n = n-1;
    ans = (n+1)*fact();
	return ans;
    }
}

int main()
{
    write("Enter a number:");
    n = read();
    if (n > 1)
    {
	result = fact();
    }
    else
    {
	result = 1;
    }
    write("The factorial is ");
    write(result);
}