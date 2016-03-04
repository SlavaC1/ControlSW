USE [RTDB]
GO
/****** Object:  Table [dbo].[Tags]    Script Date: 03/26/2015 10:47:31 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[Tags](
	[SerialNumber] [binary](8) NOT NULL,
	[Date] [date] NOT NULL,
	[PublicKey] [varbinary](max) NOT NULL,
	[Status] [tinyint] NOT NULL,
	[LastUpdateTime] [datetime] NULL,
	[Signature] [varbinary](max) NOT NULL,
 CONSTRAINT [PK_Tags_1] PRIMARY KEY CLUSTERED 
(
	[SerialNumber] ASC
)WITH (PAD_INDEX  = OFF, STATISTICS_NORECOMPUTE  = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS  = ON, ALLOW_PAGE_LOCKS  = ON) ON [PRIMARY]
) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]
GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[Materials]    Script Date: 03/26/2015 10:47:31 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[Materials](
	[ID] [smallint] NOT NULL,
	[Name] [nvarchar](32) NOT NULL,
 CONSTRAINT [PK_Materials] PRIMARY KEY CLUSTERED 
(
	[ID] ASC
)WITH (PAD_INDEX  = OFF, STATISTICS_NORECOMPUTE  = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS  = ON, ALLOW_PAGE_LOCKS  = ON) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[Batches]    Script Date: 03/26/2015 10:47:31 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[Batches](
	[BatchNumber] [nvarchar](10) NOT NULL,
	[IdtVersion] [smallint] NOT NULL,
	[MaterialID] [smallint] NOT NULL,
	[MaterialKind] [tinyint] NOT NULL,
	[ManufacturingDate] [date] NOT NULL,
	[ExpirationDate] [date] NOT NULL,
	[InitialWeight] [smallint] NOT NULL,
	[ExtraWeight] [smallint] NULL,
	[BurnStartTime] [datetime] NULL,
	[BurnEndTime] [datetime] NULL,
	[Signature] [varbinary](max) NOT NULL,
 CONSTRAINT [PK_Batches] PRIMARY KEY CLUSTERED 
(
	[BatchNumber] ASC
)WITH (PAD_INDEX  = OFF, STATISTICS_NORECOMPUTE  = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS  = ON, ALLOW_PAGE_LOCKS  = ON) ON [PRIMARY]
) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]
GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[BurningActions]    Script Date: 03/26/2015 10:47:31 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[BurningActions](
	[Id] [int] NOT NULL,
	[SerialNumber] [binary](8) NOT NULL,
	[BatchNumber] [nvarchar](10) NOT NULL,
	[StationName] [nvarchar](32) NOT NULL,
	[BurningTime] [datetime] NOT NULL,
	[IsReference] [bit] NOT NULL,
	[ErrorMessage] [nvarchar](50) NULL,
	[Signature] [varbinary](max) NOT NULL,
 CONSTRAINT [PK_BurningActions] PRIMARY KEY CLUSTERED 
(
	[Id] ASC
)WITH (PAD_INDEX  = OFF, STATISTICS_NORECOMPUTE  = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS  = ON, ALLOW_PAGE_LOCKS  = ON) ON [PRIMARY]
) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]
GO
SET ANSI_PADDING OFF
GO
/****** Object:  StoredProcedure [dbo].[spHasReferenceTags]    Script Date: 03/26/2015 10:47:30 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		<Author,,Name>
-- Create date: <Create Date,,>
-- Description:	<Description,,>
-- =============================================
CREATE PROCEDURE [dbo].[spHasReferenceTags]
	-- Add the parameters for the stored procedure here
	@batchNumber nvarchar(10)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

    -- Insert statements for procedure here
	SELECT CAST(
		CASE WHEN EXISTS(SELECT * 
		                 FROM BurningActions
		                 WHERE IsReference = 1 AND ErrorMessage IS NULL)
		THEN 1 
		ELSE 0 
		END 
	AS BIT)
END
GO
/****** Object:  StoredProcedure [dbo].[spFailuresCount]    Script Date: 03/26/2015 10:47:30 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		<Author,,Name>
-- Create date: <Create Date,,>
-- Description:	<Description,,>
-- =============================================
CREATE PROCEDURE [dbo].[spFailuresCount]
	-- Add the parameters for the stored procedure here
	@batchNumber nvarchar(10)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

    -- Insert statements for procedure here
	SELECT COUNT(*)
	FROM BurningActions
	WHERE BatchNumber = @batchNumber AND ErrorMessage IS NOT NULL
END
GO
/****** Object:  StoredProcedure [dbo].[spBurningAttemptsCount]    Script Date: 03/26/2015 10:47:30 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		<Author,,Name>
-- Create date: <Create Date,,>
-- Description:	<Description,,>
-- =============================================
CREATE PROCEDURE [dbo].[spBurningAttemptsCount]
	-- Add the parameters for the stored procedure here
	@batchNumber nvarchar(10)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

    -- Insert statements for procedure here
	SELECT COUNT(*)
	FROM BurningActions
	WHERE BatchNumber = @batchNumber
END
GO
/****** Object:  StoredProcedure [dbo].[spBurnedTagsCount]    Script Date: 03/26/2015 10:47:30 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		<Author,,Name>
-- Create date: <Create Date,,>
-- Description:	<Description,,>
-- =============================================
CREATE PROCEDURE [dbo].[spBurnedTagsCount]
	-- Add the parameters for the stored procedure here
	@batchNumber nvarchar(10)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;
	
	DECLARE @BA TABLE
	(
	  SerialNumber binary(8) not null, 
	  BurningTime datetime not null,
	  ErrorMessage nvarchar(50)
	);

	INSERT INTO @BA
		SELECT SerialNumber, MAX(BurningTime) as BurningTime, ErrorMessage
		FROM BurningActions
		WHERE BatchNumber = 'MTL6593'
		GROUP BY SerialNumber, ErrorMessage
		Having ErrorMessage IS NULL

	SELECT COUNT(*)
	FROM @BA
END
GO
/****** Object:  ForeignKey [FK_Batches_Materials]    Script Date: 03/26/2015 10:47:31 ******/
ALTER TABLE [dbo].[Batches]  WITH CHECK ADD  CONSTRAINT [FK_Batches_Materials] FOREIGN KEY([MaterialID])
REFERENCES [dbo].[Materials] ([ID])
GO
ALTER TABLE [dbo].[Batches] CHECK CONSTRAINT [FK_Batches_Materials]
GO
/****** Object:  ForeignKey [FK_BurningActions_Batches]    Script Date: 03/26/2015 10:47:31 ******/
ALTER TABLE [dbo].[BurningActions]  WITH CHECK ADD  CONSTRAINT [FK_BurningActions_Batches] FOREIGN KEY([BatchNumber])
REFERENCES [dbo].[Batches] ([BatchNumber])
GO
ALTER TABLE [dbo].[BurningActions] CHECK CONSTRAINT [FK_BurningActions_Batches]
GO
/****** Object:  ForeignKey [FK_BurningActions_Tags]    Script Date: 03/26/2015 10:47:31 ******/
ALTER TABLE [dbo].[BurningActions]  WITH CHECK ADD  CONSTRAINT [FK_BurningActions_Tags] FOREIGN KEY([SerialNumber])
REFERENCES [dbo].[Tags] ([SerialNumber])
GO
ALTER TABLE [dbo].[BurningActions] CHECK CONSTRAINT [FK_BurningActions_Tags]
GO

INSERT INTO [RTDB].[dbo].[Materials]
     VALUES
(10,	'M. Cleanser'),
(11,	'S. Cleanser'),
(430,	'Durus White'),
(450,	'RGD 450'),
(515,	'RGD 515'),
(525,	'RGD 525'),
(530,	'Tango+'),
(531,	'RGD 531'),
(535,	'RGD 535'),
(610,	'MED 610'),
(705,	'Full Cure 705'),
(707,	'SUP 707'),
(720,	'Full Cure 720'),
(810,	'Vero Clear'),
(830,	'Vero White'),
(836,	'Vero Yellow'),
(840,	'Vero Blue'),
(841,	'Vero Cyan'),
(850,	'Vero Gray'),
(851,	'Vero Magenta'),
(870,	'Vero Black'),
(950,	'Tango Gray'),
(970,	'Tango Black'),
(980,	'Tango Black+')
GO



